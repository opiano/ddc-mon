import { reactive, ref, readonly } from 'vue'
import mqtt from 'mqtt'

// Global state so connection is shared across all components
const isConnected = ref(false)
const brokerUrl = ref(localStorage.getItem('mqtt_broker_url') || 'ws://192.168.219.128:9001')
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
  SYS: null
})

let client = null
const activeSubscriptions = new Set()

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
      const updateMatch = topic.match(/^bacnet\/objects\/update\/(.+)$/)
      if (updateMatch) {
        try {
          const type = updateMatch[1]
          const payload = JSON.parse(message.toString())
          
          if (type === 'SYS') {
             bacnetData.SYS = payload
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
    writeValue
  }
}
