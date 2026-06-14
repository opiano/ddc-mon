import { reactive, ref, readonly } from 'vue'
import mqtt from 'mqtt'

// Global state so connection is shared across all components
const isConnected = ref(false)
const brokerUrl = ref(localStorage.getItem('mqtt_broker_url') || `ws://${window.location.hostname}:9001`)
const bacnetData = reactive({
  AI: [],
  AO: [],
  AV: [],
  BI: [],
  BO: [],
  BV: [],
  MSI: [],
  MSO: [],
  MSV: [],
  CAL: [],
  SCH: [],
  TLOG: [],
  FBD: [],
  DEV: null,
  DEVLST: []
})

const trendData = reactive({})

let client = null
const activeSubscriptions = new Set()

// Helper to sanitize malformed JSON from MQTT, including trailing commas and mismatched brackets/braces (e.g., arrays ending in } instead of ])
function sanitizeMqttMessage(rawMessage) {
  // 1. Initial cleanup of simple trailing commas
  let sanitized = rawMessage.replace(/,\s*([\]}])/g, '$1');

  // 2. Fix events array closing bracket mismatch if it exists (e.g. "events": [ ... } )
  const eventsIndex = sanitized.indexOf('"events"');
  if (eventsIndex !== -1) {
    const startBracketIndex = sanitized.indexOf('[', eventsIndex);
    if (startBracketIndex !== -1) {
      let bracketCount = 1;
      let braceCount = 0;
      let inString = false;
      let escape = false;

      for (let i = startBracketIndex + 1; i < sanitized.length; i++) {
        const char = sanitized[i];
        if (escape) {
          escape = false;
          continue;
        }
        if (char === '\\') {
          escape = true;
          continue;
        }
        if (char === '"') {
          inString = !inString;
          continue;
        }
        if (inString) continue;

        if (char === '{') {
          braceCount++;
        } else if (char === '}') {
          if (braceCount > 0) {
            braceCount--;
          } else {
            // Found unmatched closing brace instead of a bracket. Correct it.
            sanitized = sanitized.substring(0, i) + ']' + sanitized.substring(i + 1);
            bracketCount = 0;
            break;
          }
        } else if (char === '[') {
          bracketCount++;
        } else if (char === ']') {
          bracketCount--;
          if (bracketCount === 0) {
            break;
          }
        }
      }
    }
  }

  // 3. Final pass to strip any remaining trailing commas
  sanitized = sanitized.replace(/,\s*([\]}])/g, '$1');
  return sanitized;
}

export function useMqtt() {
  const connect = () => {
    if (client) return // Already connecting or connected

    // Attempt to connect
    client = mqtt.connect(brokerUrl.value, {
      clean: true,
      connectTimeout: 5000,
      clientId: 'bacnet_web_' + Math.random().toString(16).substr(2, 8),
    })

    client.on('connect', () => {
      isConnected.value = true
      console.log('Connected to MQTT Broker at', brokerUrl.value)
      
      // Resubscribe to active topics on reconnect
      activeSubscriptions.forEach(type => {
        const topic = `bacnet/objects/update/${type}`
        client.subscribe(topic, (err) => {
          if (!err) {
            console.log(`Resubscribed to ${topic}`)
            // Notify C backend to start sending data for this type
            client.publish(`bacnet/request/subscribe/${type}`, '', { qos: 0 })
          }
        })
      })
    })

    client.on('message', (topic, message) => {
      const trendMatch = topic.match(/^bacnet\/objects\/trend\/(.+)$/)
      if (trendMatch) {
        try {
          const tlogId = trendMatch[1]
          let rawMessage = message.toString().trim()
          
          // 1. Fix missing commas between objects: "}{" -> "},{"
          rawMessage = rawMessage.replace(/\}\s*\{/g, '},{')
          
          // 2. Fix trailing commas before closing brackets/braces: e.g. ",]" or ",, ]" -> "]" or " ]"
          rawMessage = rawMessage.replace(/,+(?=\s*[\]}])/g, '')

          const payload = JSON.parse(rawMessage)
          trendData[tlogId] = payload
        } catch (e) {
          console.error(`Failed to parse trend MQTT message on ${topic}:`, e)
          const msgStr = message.toString()
          console.log(`Raw message length: ${msgStr.length}`)
          console.log(`Snippet around pos 4654: "${msgStr.substring(Math.max(0, 4654 - 50), Math.min(msgStr.length, 4654 + 50))}"`)
        }
        return
      }

      const updateMatch = topic.match(/^bacnet\/objects\/update\/(.+)$/)
      if (updateMatch) {
        try {
          const type = updateMatch[1]
          let rawMessage = message.toString().trim()

          // 1. If payload contains multiple objects separated by commas but lacks outer array brackets, wrap them
          if (type !== 'DEV' && rawMessage.startsWith('{') && !rawMessage.startsWith('[')) {
            if (rawMessage.endsWith(',')) {
              rawMessage = rawMessage.slice(0, -1)
            }
            rawMessage = '[' + rawMessage + ']'
          }

          // 2. Robustly sanitize message for trailing commas and mismatched array closing brackets
          const sanitizedMessage = sanitizeMqttMessage(rawMessage)

          const payload = JSON.parse(sanitizedMessage)
          
          if (type === 'DEV') {
             bacnetData.DEV = payload
          } else if (bacnetData[type] !== undefined && Array.isArray(payload)) {
             bacnetData[type] = payload
          } else if (payload.type && Array.isArray(payload.objects)) {
             if (bacnetData[payload.type] !== undefined) {
                bacnetData[payload.type] = payload.objects
             }
          }
        } catch (e) {
          console.error(`Failed to parse MQTT message on ${topic}:`, e)
        }
      }
    })

    client.on('error', (err) => {
      console.error('MQTT Connection Error:', err)
    })

    client.on('close', () => {
      isConnected.value = false
      console.log('Disconnected from MQTT Broker')
    })
  }

  const disconnect = () => {
    if (client) {
      client.end()
      client = null
    }
  }

  const setBrokerUrl = (newUrl) => {
    brokerUrl.value = newUrl
    localStorage.setItem('mqtt_broker_url', newUrl)
    
    // Disconnect existing client and reconnect with new URL
    disconnect()
    connect()
  }

  const subscribeToType = (type) => {
    activeSubscriptions.add(type)
    if (client && isConnected.value) {
      const topic = `bacnet/objects/update/${type}`
      client.subscribe(topic, (err) => {
        if (!err) {
          console.log(`Subscribed to ${topic}`)
          // Notify C backend to start sending data for this type
          publish(`bacnet/request/subscribe/${type}`, '')
        }
        else console.error(`Failed to subscribe to ${topic}`, err)
      })
    }
  }

  const unsubscribeFromType = (type) => {
    activeSubscriptions.delete(type)
    if (client && isConnected.value) {
      const topic = `bacnet/objects/update/${type}`
      client.unsubscribe(topic, (err) => {
        if (!err) {
          console.log(`Unsubscribed from ${topic}`)
          // Notify C backend to stop sending data for this type
          publish(`bacnet/request/unsubscribe/${type}`, '')
        }
      })
    }
  }

  const publish = (topic, message) => {
    if (client && isConnected.value) {
      const payload = typeof message === 'string' ? message : JSON.stringify(message)
      client.publish(topic, payload, { qos: 0 }, (err) => {
        if (err) {
          console.error(`Failed to publish to ${topic}:`, err)
        } else {
          console.log(`Published to ${topic}:`, payload)
        }
      })
    } else {
      console.warn('MQTT client is not connected. Cannot publish message.')
    }
  }

  const writeValue = (type, id, value, priority = 16) => {
    const topic = `bacnet/command/write/${type}/${id}`
    const message = `${value},${priority}`
    publish(topic, message)
  }

  const writeAlarmConfig = (type, id, payload) => {
    const topic = `bacnet/command/alarm/${type}/${id}`
    publish(topic, payload)
  }

  const subscribeToTrend = (tlogId) => {
    if (client && isConnected.value) {
      const topic = `bacnet/objects/trend/${tlogId}`
      client.subscribe(topic, (err) => {
        if (!err) {
          console.log(`Subscribed to trend topic: ${topic}`)
          // Publish request to backend
          publish(`bacnet/request/trend/${tlogId}`, '')
        } else {
          console.error(`Failed to subscribe to ${topic}`, err)
        }
      })
    }
  }

  const unsubscribeFromTrend = (tlogId) => {
    if (client && isConnected.value) {
      const topic = `bacnet/objects/trend/${tlogId}`
      client.unsubscribe(topic, (err) => {
        if (!err) {
          console.log(`Unsubscribed from trend topic: ${topic}`)
          delete trendData[tlogId]
        }
      })
    }
  }

  // Automatically connect the first time this composable is used
  if (!client) {
    connect()
  }

  return {
    isConnected: readonly(isConnected),
    brokerUrl: readonly(brokerUrl),
    bacnetData: readonly(bacnetData),
    connect,
    disconnect,
    setBrokerUrl,
    subscribeToType,
    unsubscribeFromType,
    publish,
    writeValue,
    writeAlarmConfig,
    trendData: readonly(trendData),
    subscribeToTrend,
    unsubscribeFromTrend
  }
}
