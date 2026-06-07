<script setup>
import { useMqtt } from '../composables/useMqtt'
import { computed, onMounted, onUnmounted, ref } from 'vue'

const { bacnetData, subscribeToType, unsubscribeFromType, publish } = useMqtt()

// Device and Time modal states
const showDeviceModal = ref(false)
const inputDeviceNumber = ref(100)
const showTimeModal = ref(false)
const inputSystemTime = ref('')
const showRecipientModal = ref(false)
const inputRecipient1 = ref('')
const inputRecipient2 = ref('')
const errorRecipient1 = ref('')
const errorRecipient2 = ref('')

const openDeviceModal = () => {
  let instanceStr = sysData.value.bacnetInstance || ''
  let num = parseInt(instanceStr.replace(/[^0-9]/g, ''), 10)
  if (isNaN(num)) num = 100
  inputDeviceNumber.value = num
  showDeviceModal.value = true
}

const saveDeviceNumber = () => {
  if (inputDeviceNumber.value !== null && inputDeviceNumber.value !== undefined) {
    publish('bacnet/command/device/instance', inputDeviceNumber.value.toString())
  }
  showDeviceModal.value = false
}

const closeDeviceModal = () => {
  showDeviceModal.value = false
}

const openTimeModal = () => {
  let timeStr = sysData.value.systemTime || ''
  if (timeStr && timeStr.match(/^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}$/)) {
    inputSystemTime.value = timeStr.substring(0, 10) + 'T' + timeStr.substring(11, 16)
  } else {
    const now = new Date()
    const offset = now.getTimezoneOffset()
    const localNow = new Date(now.getTime() - offset * 60 * 1000)
    inputSystemTime.value = localNow.toISOString().substring(0, 16)
  }
  showTimeModal.value = true
}

const setSystemTimeToNow = () => {
  const now = new Date()
  const offset = now.getTimezoneOffset()
  const localNow = new Date(now.getTime() - offset * 60 * 1000)
  inputSystemTime.value = localNow.toISOString().substring(0, 16)
}

const saveSystemTime = () => {
  if (inputSystemTime.value) {
    const formattedTime = inputSystemTime.value.replace('T', ' ') + ':00'
    publish('bacnet/command/system/time', formattedTime)
  }
  showTimeModal.value = false
}

const closeTimeModal = () => {
  showTimeModal.value = false
}

const openRecipientModal = () => {
  inputRecipient1.value = ''
  inputRecipient2.value = ''
  errorRecipient1.value = ''
  errorRecipient2.value = ''
  showRecipientModal.value = true
}

const validateRecipientValue = (val) => {
  const trimmed = val.trim()
  if (!trimmed) return ''
  
  if (/^\d+$/.test(trimmed)) {
    const num = parseInt(trimmed, 10)
    if (num > 4194303) {
      return 'Device Instance는 0 ~ 4194303 사이의 숫자여야 합니다.'
    }
    return ''
  }
  
  const match = trimmed.match(/^(\d+)\/(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}):(\d+)$/)
  if (match) {
    const netNum = parseInt(match[1], 10)
    if (netNum < 1 || netNum > 65535) {
      return 'Network Number는 1 ~ 65535 사이의 숫자여야 합니다.'
    }
    
    const octets = match[2].split('.').map(o => parseInt(o, 10))
    if (octets.some(o => o > 255)) {
      return 'IP 주소의 각 자리는 0 ~ 255 사이여야 합니다.'
    }
    
    const port = parseInt(match[3], 10)
    if (port < 1 || port > 65535) {
      return 'Port는 1 ~ 65535 사이의 숫자여야 합니다.'
    }
    
    return ''
  }
  
  return '올바른 형식이 아닙니다. 숫자 또는 숫자/IP:Port 형식으로 입력해주세요. (예: 123 또는 1/192.168.219.13:47808)'
}

const saveRecipients = () => {
  errorRecipient1.value = validateRecipientValue(inputRecipient1.value)
  errorRecipient2.value = validateRecipientValue(inputRecipient2.value)
  
  if (errorRecipient1.value || errorRecipient2.value) {
    return
  }

  const list = []
  if (inputRecipient1.value.trim()) {
    list.push(inputRecipient1.value.trim())
  }
  if (inputRecipient2.value.trim()) {
    list.push(inputRecipient2.value.trim())
  }
  const payload = list.join(',')
  publish('bacnet/command/device/recipient', payload)
  showRecipientModal.value = false
}

const closeRecipientModal = () => {
  showRecipientModal.value = false
}

onMounted(() => subscribeToType('DEV'))
onUnmounted(() => unsubscribeFromType('DEV'))

const sysData = computed(() => bacnetData.DEV || {
  bacnetInstance: '-',
  systemStatus: '-',
  totalObjects: 0,
  modules: 0,
  activeAlarms: 0,
  cpuUsage: 0,
  memoryUsage: 0,
  cpuTemp: 0,
  hostname: '-',
  systemTime: '-',
  uptime: 0,
  bootTime: '-',
  recipients: [],
  events: []
})

const formatUptime = (seconds) => {
  if (seconds === undefined || seconds === null) return '-';
  const d = Math.floor(seconds / (3600*24));
  const h = Math.floor(seconds % (3600*24) / 3600);
  const m = Math.floor(seconds % 3600 / 60);
  const s = Math.floor(seconds % 60);
  return `${d} days, ${h} hours, ${m} min, ${s} sec`;
}

const activeTempColor = computed(() => {
  const temp = sysData.value.cpuTemp || 0
  if (temp >= 70) return '#ee7d77' // Danger (Error Red)
  if (temp >= 50) return '#ffb148' // Warning (Tertiary Amber)
  return '#10b981' // Normal (Emerald Green)
})

const sortedEvents = computed(() => {
  if (!sysData.value.events || !Array.isArray(sysData.value.events)) {
    return []
  }
  return [...sysData.value.events]
    .filter(e => e && e.timestamp)
    .sort((a, b) => {
      const dateA = new Date(a.timestamp.replace(/-/g, '/'))
      const dateB = new Date(b.timestamp.replace(/-/g, '/'))
      return dateB.getTime() - dateA.getTime()
    })
    .slice(0, 5)
})

const exportAlarms = () => {
  if (!sysData.value.events || !Array.isArray(sysData.value.events)) {
    alert('No active alarms to export.')
    return
  }
  
  const eventsToExport = [...sysData.value.events]
    .filter(e => e && e.timestamp)
    .sort((a, b) => {
      const dateA = new Date(a.timestamp.replace(/-/g, '/'))
      const dateB = new Date(b.timestamp.replace(/-/g, '/'))
      return dateB.getTime() - dateA.getTime()
    })

  if (eventsToExport.length === 0) {
    alert('No active alarms to export.')
    return
  }

  // Build CSV content
  const headers = ['Timestamp', 'Event State', 'Object Name', 'Object ID']
  const rows = eventsToExport.map(e => [
    `"${e.timestamp || ''}"`,
    `"${e.eventState || ''}"`,
    `"${e.objectName || ''}"`,
    `"${e.objectId || ''}"`
  ])
  
  const csvContent = '\uFEFF' + [headers.join(','), ...rows.map(r => r.join(','))].join('\n') // UTF-8 BOM for Excel compatibility in Korean
  
  const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8;' })
  const url = URL.createObjectURL(blob)
  const link = document.createElement('a')
  link.setAttribute('href', url)
  link.setAttribute('download', `active_alarms_${new Date().toISOString().split('T')[0]}.csv`)
  link.style.visibility = 'hidden'
  document.body.appendChild(link)
  link.click()
  document.body.removeChild(link)
}
</script>
<template>
  <div class="p-4 flex-1 overflow-auto bg-surface-dim">
    <!-- System Health Header -->
    <header class="flex flex-col md:flex-row md:items-center justify-between mb-4 gap-4">
    <div>
    <h1 class="text-2xl font-bold font-space tracking-tight text-on-surface mb-1">System Overview</h1>
    <p class="text-secondary font-label text-sm uppercase tracking-widest">REAL-TIME MONITOR</p>
    </div>
    <div class="flex gap-4">
    <div @click="openDeviceModal" class="bg-surface-container-low px-4 py-2 rounded flex flex-col items-end border-l-4 border-primary cursor-pointer hover:bg-surface-container-high transition-all group select-none">
    <div class="flex items-center gap-1">
      <span class="material-symbols-outlined text-xs text-primary/50 group-hover:text-primary transition-colors">edit</span>
      <span class="text-primary font-space text-xl font-bold">{{ sysData.bacnetInstance }}</span>
    </div>
    <span class="text-[10px] text-secondary-dim font-label uppercase">BACnet Instance</span>
    </div>
    <div class="bg-surface-container-low px-4 py-2 rounded flex flex-col items-end border-l-4 border-emerald-500">
    <span class="text-emerald-400 font-space text-xl font-bold">{{ sysData.systemStatus }}</span>
    <span class="text-[10px] text-secondary-dim font-label uppercase">System Status</span>
    </div>
    </div>
    </header>
    <!-- Bento Grid Main Content -->
    <div class="grid grid-cols-12 gap-4">
    <!-- Metric Cards (Row 1) -->
    <div class="col-span-12 md:col-span-4 bg-surface-container-high p-4 border-l-2 border-outline-variant/20 hover:bg-surface-bright transition-colors duration-300">
    <div class="flex justify-between items-start mb-2">
    <span class="material-symbols-outlined text-primary text-3xl">hub</span>
    </div>
    <div class="text-3xl font-bold font-space text-on-surface">{{ sysData.totalObjects }}</div>
    <div class="text-sm text-secondary uppercase tracking-tight">Total Objects</div>
    </div>
    <div class="col-span-12 md:col-span-4 bg-surface-container-high p-4 border-l-2 border-outline-variant/20 hover:bg-surface-bright transition-colors duration-300">
    <div class="flex justify-between items-start mb-2">
    <span class="material-symbols-outlined text-primary text-3xl">dns</span>
    </div>
    <div class="text-3xl font-bold font-space text-on-surface">{{ sysData.modules }}</div>
    <div class="text-sm text-secondary uppercase tracking-tight">Modules</div>
    </div>
    <div class="col-span-12 md:col-span-4 bg-surface-container-high p-4 border-l-2 border-error/50 hover:bg-error-container/20 transition-colors duration-300">
    <div class="flex justify-between items-start mb-2">
    <span class="material-symbols-outlined text-error text-3xl">warning</span>
    </div>
    <div class="text-3xl font-bold font-space text-error">{{ sysData.activeAlarms }}</div>
    <div class="text-sm text-on-error-container uppercase tracking-tight">Active Alarms</div>
    </div>
    <!-- Alarm Distribution & Resources (Row 2) -->
    <div class="col-span-12 lg:col-span-5 bg-surface-container-low p-4 flex flex-col">
    <div class="flex justify-between items-center mb-4">
    <h3 class="font-space text-base font-bold uppercase tracking-wider text-on-surface">System Monitor</h3>
    </div>
    <div class="grid grid-cols-3 gap-3 flex-1 items-center pb-4">
    <!-- Custom Gauge Implementation -->
    <div class="flex flex-col items-center justify-center relative">
    <svg class="w-28 h-28 sm:w-32 sm:h-32 lg:w-36 lg:h-36 transform -rotate-90" viewBox="0 0 128 128">
    <circle class="text-surface-container-highest" cx="64" cy="64" fill="transparent" r="54" stroke="currentColor" stroke-width="8"></circle>
    <circle class="text-primary" cx="64" cy="64" fill="transparent" r="54" stroke="currentColor" stroke-dasharray="339.29" :stroke-dashoffset="339.29 - (339.29 * sysData.cpuUsage / 100)" stroke-width="8"></circle>
    </svg>
    <div class="absolute inset-0 flex flex-col items-center justify-center mt-[-4px]">
    <span class="text-xl sm:text-2xl font-bold font-space">{{ sysData.cpuUsage }}%</span>
    <span class="text-[9px] sm:text-[10px] text-secondary-dim font-bold uppercase">CPU</span>
    </div>
    </div>
    <div class="flex flex-col items-center justify-center relative">
    <svg class="w-28 h-28 sm:w-32 sm:h-32 lg:w-36 lg:h-36 transform -rotate-90" viewBox="0 0 128 128">
    <circle class="text-surface-container-highest" cx="64" cy="64" fill="transparent" r="54" stroke="currentColor" stroke-width="8"></circle>
    <circle class="text-primary" cx="64" cy="64" fill="transparent" r="54" stroke="currentColor" stroke-dasharray="339.29" :stroke-dashoffset="339.29 - (339.29 * sysData.memoryUsage / 100)" stroke-width="8"></circle>
    </svg>
    <div class="absolute inset-0 flex flex-col items-center justify-center mt-[-4px]">
    <span class="text-xl sm:text-2xl font-bold font-space">{{ sysData.memoryUsage }}%</span>
    <span class="text-[9px] sm:text-[10px] text-secondary-dim font-bold uppercase">Memory</span>
    </div>
    </div>
    <!-- CPU Temperature 240-degree Needle Gauge -->
    <div class="flex flex-col items-center justify-center relative">
    <svg class="w-28 h-28 sm:w-32 sm:h-32 lg:w-36 lg:h-36" viewBox="0 0 128 128">
    <!-- 240-degree circular base track split into gorgeous colored segments -->
    <!-- Normal Range (0 to 50°C): Emerald Green -->
    <path d="M 17.24,91 A 54,54 0 0,1 64,10" fill="transparent" stroke="#10b981" stroke-width="8" stroke-linecap="round" class="opacity-20" />
    <!-- Warning Range (50 to 70°C): Tertiary Amber -->
    <path d="M 64,10 A 54,54 0 0,1 104.13,27.87" fill="transparent" stroke="#ffb148" stroke-width="8" class="opacity-20" />
    <!-- Danger Range (70 to 100°C): Error Red -->
    <path d="M 104.13,27.87 A 54,54 0 0,1 110.76,91" fill="transparent" stroke="#ee7d77" stroke-width="8" stroke-linecap="round" class="opacity-20" />
    
    <!-- Active Progress Segments overlayed dynamically -->
    <!-- Normal Range Active (0 to 50°C): Emerald Green -->
    <path d="M 17.24,91 A 54,54 0 0,1 64,10" fill="transparent" stroke="#10b981" stroke-dasharray="113.10" :stroke-dashoffset="113.10 - (113.10 * Math.min(Math.max(sysData.cpuTemp || 0, 0), 50) / 50)" stroke-width="8" stroke-linecap="round" class="transition-all duration-500 ease-out" />
    <!-- Warning Range Active (50 to 70°C): Tertiary Amber -->
    <path d="M 64,10 A 54,54 0 0,1 104.13,27.87" fill="transparent" stroke="#ffb148" stroke-dasharray="45.24" :stroke-dashoffset="45.24 - (45.24 * Math.min(Math.max((sysData.cpuTemp || 0) - 50, 0), 20) / 20)" stroke-width="8" class="transition-all duration-500 ease-out" />
    <!-- Danger Range Active (70 to 100°C): Error Red -->
    <path d="M 104.13,27.87 A 54,54 0 0,1 110.76,91" fill="transparent" stroke="#ee7d77" stroke-dasharray="67.85" :stroke-dashoffset="67.85 - (67.85 * Math.min(Math.max((sysData.cpuTemp || 0) - 70, 0), 30) / 30)" stroke-width="8" stroke-linecap="round" class="transition-all duration-500 ease-out" />
    
    <!-- Gauge Ticks -->
    <line x1="17.24" y1="91" x2="24.2" y2="87" stroke="#8f9fb7" stroke-width="2"/>
    <line x1="64" y1="10" x2="64" y2="18" stroke="#8f9fb7" stroke-width="2"/>
    <line x1="110.76" y1="91" x2="103.8" y2="87" stroke="#8f9fb7" stroke-width="2"/>
    
    <!-- Gauge Numbers inside the dial -->
    <text x="34" y="86" fill="#8f9fb7" font-size="9" text-anchor="middle" font-family="Space Grotesk" class="font-bold opacity-60">0</text>
    <text x="64" y="32" fill="#8f9fb7" font-size="9" text-anchor="middle" font-family="Space Grotesk" class="font-bold opacity-60">50</text>
    <text x="94" y="86" fill="#8f9fb7" font-size="9" text-anchor="middle" font-family="Space Grotesk" class="font-bold opacity-60">100</text>
    
    <!-- Rotating needle pointing to temp. Range 0 to 100 -> angle -120deg to 120deg (240deg total). Tip is at y=24 (radius 40) for a cleaner sweep. -->
    <polygon points="61,64 64,24 67,64" :fill="activeTempColor" :style="{ transform: `rotate(${-120 + Math.min(Math.max(sysData.cpuTemp || 0, 0), 100) * 2.4}deg)`, transformOrigin: '64px 64px' }" class="transition-transform duration-500 ease-out" />
    
    <!-- Center Hub -->
    <circle cx="64" cy="64" r="8" fill="#dee5ff" stroke="#060e20" stroke-width="2"/>
    <circle cx="64" cy="64" r="3" :fill="activeTempColor"/>
    </svg>
    <div class="absolute inset-x-0 bottom-[-2px] flex flex-col items-center">
    <span class="text-[9px] sm:text-[10px] text-secondary-dim font-bold uppercase tracking-wider leading-none mb-0.5">Temp</span>
    <span class="text-base sm:text-lg font-bold font-space leading-none pb-1 transition-colors duration-300" :style="{ color: activeTempColor }" :class="{ 'animate-pulse': (sysData.cpuTemp || 0) >= 70 }">{{ sysData.cpuTemp ? sysData.cpuTemp.toFixed(1) : '-' }}°C</span>
    </div>
    </div>
    </div>
    </div>
    <div class="col-span-12 lg:col-span-7 bg-surface-container-low p-4">
      <div class="flex justify-between items-center mb-4">
        <h3 class="font-space text-base font-bold uppercase tracking-wider text-on-surface">System Information</h3>
        <span class="material-symbols-outlined text-slate-500 text-sm">info</span>
      </div>
      <div class="flex flex-col gap-2">
        <div class="flex justify-between items-center py-1 border-b border-outline-variant/10">
          <span class="text-xs font-label uppercase text-slate-400">Hostname</span>
          <span class="font-mono text-sm text-on-surface">{{ sysData.hostname }}</span>
        </div>
        <div class="flex justify-between items-center py-1 border-b border-outline-variant/10">
          <span class="text-xs font-label uppercase text-slate-400">System Time</span>
          <div class="flex items-center gap-3">
            <button @click="openTimeModal" class="px-2.5 py-1 text-xs font-bold text-primary bg-primary/10 hover:bg-primary/20 rounded border border-primary/20 transition-all flex items-center gap-1.5 shadow-sm">
              <span class="material-symbols-outlined text-xs">schedule</span> Set
            </button>
            <span class="font-mono text-sm text-on-surface">{{ sysData.systemTime }}</span>
          </div>
        </div>
        <div class="flex justify-between items-center py-1 border-b border-outline-variant/10">
          <span class="text-xs font-label uppercase text-slate-400">Boot Time</span>
          <span class="font-mono text-sm text-on-surface">{{ sysData.bootTime }}</span>
        </div>
        <div class="flex justify-between items-center py-1 border-b border-outline-variant/10">
          <span class="text-xs font-label uppercase text-slate-400">Up Time</span>
          <div class="flex flex-col items-end">
            <span class="font-mono text-sm text-on-surface">{{ sysData.uptime }} seconds</span>
            <span class="text-[10px] font-bold text-primary px-1 py-0.5 bg-primary/10 rounded">{{ formatUptime(sysData.uptime) }}</span>
          </div>
        </div>
        <div class="flex justify-between items-center py-1">
          <span class="text-xs font-label uppercase text-slate-400">Recipient</span>
          <div class="flex items-center gap-3">
            <button @click="openRecipientModal" class="px-2.5 py-1 text-xs font-bold text-primary bg-primary/10 hover:bg-primary/20 rounded border border-primary/20 transition-all flex items-center gap-1.5 shadow-sm">
              <span class="material-symbols-outlined text-xs">settings</span> Set
            </button>
            <span class="font-mono text-sm text-on-surface">{{ sysData.recipients && sysData.recipients.length > 0 ? sysData.recipients.join(', ') : '-' }}</span>
          </div>
        </div>
      </div>
    </div>
    <!-- Recent Events Log (Row 3) -->
    <div class="col-span-12 bg-surface-container-low overflow-hidden">
    <div class="px-4 py-3 border-b border-outline-variant/10 flex justify-between items-center">
    <h3 class="font-space text-lg font-bold uppercase tracking-wider text-on-surface">Recent Events Log</h3>
    <div class="flex gap-4">
    <button @click="exportAlarms" class="text-xs font-bold text-secondary-dim flex items-center gap-2 px-3 py-1 bg-surface-container-highest rounded border border-outline-variant/20 hover:text-on-surface transition-colors">
    <span class="material-symbols-outlined text-sm">download</span> Export
    </button>
    </div>
    </div>
    <div class="overflow-x-auto">
    <table class="w-full text-left font-label">
    <thead>
    <tr class="bg-surface-container-high text-xs uppercase tracking-widest text-secondary-dim">
    <th class="text-sm font-bold text-slate-500 uppercase tracking-widest py-2 pl-4">Timestamp</th>
    <th class="text-sm font-bold text-slate-500 uppercase tracking-widest py-2 pl-4">Event State</th>
    <th class="text-sm font-bold text-slate-500 uppercase tracking-widest py-2 pl-4">Object Name</th>
    <th class="text-sm font-bold text-slate-500 uppercase tracking-widest py-2 pl-4">Object Id</th>
    </tr>
    </thead>
    <tbody class="divide-y divide-outline-variant/5">
      <tr v-for="(event, index) in sortedEvents" :key="index" :class="index % 2 === 1 ? 'bg-surface-container-lowest/5 hover:bg-surface-container-highest/30 transition-colors' : 'hover:bg-surface-container-highest/30 transition-colors'">
        <td class="px-4 py-2 text-sm font-medium text-on-surface/80">{{ event.timestamp }}</td>
        <td class="px-4 py-2">
          <span class="text-[10px] font-bold px-2 py-0.5 rounded border uppercase" :class="{
            'bg-error/10 text-error border-error/20': event.eventState === 'Critical',
            'bg-tertiary/10 text-tertiary border-tertiary/20': event.eventState === 'Warning',
            'bg-primary/10 text-primary border-primary/20': event.eventState === 'Info' || event.eventState === 'Normal'
          }">{{ event.eventState }}</span>
        </td>
        <td class="px-4 py-2 text-sm text-on-surface">{{ event.objectName }}</td>
        <td class="px-4 py-2 text-sm font-mono text-primary-dim">{{ event.objectId }}</td>
      </tr>
      <tr v-if="sortedEvents.length === 0">
        <td colspan="4" class="px-4 py-2 text-sm text-slate-500 text-center">No recent events</td>
      </tr>
    </tbody>
    </table>
    </div>
    </div>
    </div>
  </div>

  <!-- Device Settings Modal -->
  <div v-if="showDeviceModal" class="fixed inset-0 z-[100] flex items-center justify-center bg-black/60 backdrop-blur-sm p-4">
    <div class="bg-surface-container-high rounded-lg shadow-2xl border border-outline-variant/20 w-full max-w-md overflow-hidden transform transition-all animate-in fade-in zoom-in-95 duration-200">
      <div class="px-6 py-4 border-b border-outline-variant/10 flex justify-between items-center bg-surface-container-low">
        <h2 class="text-lg font-bold text-on-surface flex items-center gap-2 font-space">
          <span class="material-symbols-outlined text-primary text-xl">hub</span>
          Device Instance Setting
        </h2>
        <button @click="closeDeviceModal" class="text-slate-400 hover:text-white transition-colors">
          <span class="material-symbols-outlined">close</span>
        </button>
      </div>
      <div class="p-6 space-y-4">
        <div>
          <label class="block text-xs font-bold text-slate-400 mb-2 uppercase tracking-wider font-label">Device Number (Instance ID)</label>
          <input type="number" v-model="inputDeviceNumber" class="w-full bg-surface-container-highest border border-outline-variant/20 rounded px-3 py-2 text-on-surface font-mono text-sm focus:outline-none focus:border-primary transition-colors" />
          <p class="text-xs text-slate-500 mt-2">Enter a number between 0 and 4194303.</p>
        </div>
      </div>
      <div class="px-6 py-4 bg-surface-container-low flex justify-end gap-3 border-t border-outline-variant/10">
        <button @click="closeDeviceModal" class="px-4 py-2 rounded text-sm font-bold text-slate-300 hover:bg-surface-container-highest transition-colors">Cancel</button>
        <button @click="saveDeviceNumber" class="px-4 py-2 rounded text-sm font-bold bg-primary text-on-primary hover:brightness-110 transition-colors shadow-lg shadow-primary/20">Set</button>
      </div>
    </div>
  </div>

  <!-- System Time Settings Modal -->
  <div v-if="showTimeModal" class="fixed inset-0 z-[100] flex items-center justify-center bg-black/60 backdrop-blur-sm p-4">
    <div class="bg-surface-container-high rounded-lg shadow-2xl border border-outline-variant/20 w-full max-w-md overflow-hidden transform transition-all animate-in fade-in zoom-in-95 duration-200">
      <div class="px-6 py-4 border-b border-outline-variant/10 flex justify-between items-center bg-surface-container-low">
        <h2 class="text-lg font-bold text-on-surface flex items-center gap-2 font-space">
          <span class="material-symbols-outlined text-primary text-xl">schedule</span>
          System Time Setting
        </h2>
        <button @click="closeTimeModal" class="text-slate-400 hover:text-white transition-colors">
          <span class="material-symbols-outlined">close</span>
        </button>
      </div>
      <div class="p-6 space-y-4">
        <div>
          <label class="block text-xs font-bold text-slate-400 mb-2 uppercase tracking-wider font-label">Select Date & Time</label>
          <input type="datetime-local" v-model="inputSystemTime" class="w-full bg-surface-container-highest border border-outline-variant/20 rounded px-3 py-2 text-on-surface font-mono text-sm focus:outline-none focus:border-primary transition-colors [color-scheme:dark]" />
          <div class="flex gap-2 mt-2">
            <button @click="setSystemTimeToNow" class="text-xs text-primary font-bold hover:underline">Use Current Time</button>
          </div>
        </div>
      </div>
      <div class="px-6 py-4 bg-surface-container-low flex justify-end gap-3 border-t border-outline-variant/10">
        <button @click="closeTimeModal" class="px-4 py-2 rounded text-sm font-bold text-slate-300 hover:bg-surface-container-highest transition-colors">Cancel</button>
        <button @click="saveSystemTime" class="px-4 py-2 rounded text-sm font-bold bg-primary text-on-primary hover:brightness-110 transition-colors shadow-lg shadow-primary/20">Set</button>
      </div>
    </div>
  </div>

  <!-- Recipient Settings Modal -->
  <div v-if="showRecipientModal" class="fixed inset-0 z-[100] flex items-center justify-center bg-black/60 backdrop-blur-sm p-4">
    <div class="bg-surface-container-high rounded-lg shadow-2xl border border-outline-variant/20 w-full max-w-md overflow-hidden transform transition-all animate-in fade-in zoom-in-95 duration-200">
      <div class="px-6 py-4 border-b border-outline-variant/10 flex justify-between items-center bg-surface-container-low">
        <h2 class="text-lg font-bold text-on-surface flex items-center gap-2 font-space">
          <span class="material-symbols-outlined text-primary text-xl">settings</span>
          Recipient Setting
        </h2>
        <button @click="closeRecipientModal" class="text-slate-400 hover:text-white transition-colors">
          <span class="material-symbols-outlined">close</span>
        </button>
      </div>
      <div class="p-6 space-y-4">
        <div>
          <label class="block text-xs font-bold text-slate-400 mb-2 uppercase tracking-wider font-label">Recipient 1</label>
          <input type="text" v-model="inputRecipient1" placeholder="e.g., 123" 
            :class="[
              'w-full bg-surface-container-highest border rounded px-3 py-2 text-on-surface font-mono text-sm focus:outline-none transition-colors',
              errorRecipient1 ? 'border-error/50 focus:border-error' : 'border-outline-variant/20 focus:border-primary'
            ]" 
          />
          <p v-if="errorRecipient1" class="text-xs text-error mt-1.5 flex items-center gap-1">
            <span class="material-symbols-outlined text-[14px]">error</span>
            {{ errorRecipient1 }}
          </p>
        </div>
        <div>
          <label class="block text-xs font-bold text-slate-400 mb-2 uppercase tracking-wider font-label">Recipient 2</label>
          <input type="text" v-model="inputRecipient2" placeholder="e.g., 1/192.168.219.13:47808" 
            :class="[
              'w-full bg-surface-container-highest border rounded px-3 py-2 text-on-surface font-mono text-sm focus:outline-none transition-colors',
              errorRecipient2 ? 'border-error/50 focus:border-error' : 'border-outline-variant/20 focus:border-primary'
            ]" 
          />
          <p v-if="errorRecipient2" class="text-xs text-error mt-1.5 flex items-center gap-1">
            <span class="material-symbols-outlined text-[14px]">error</span>
            {{ errorRecipient2 }}
          </p>
        </div>
        <p class="text-xs text-slate-500">
          Enter up to 2 recipients. You can enter a Device Instance ID or a Network / IP:PORT address. Leave empty to clear.
        </p>
      </div>
      <div class="px-6 py-4 bg-surface-container-low flex justify-end gap-3 border-t border-outline-variant/10">
        <button @click="closeRecipientModal" class="px-4 py-2 rounded text-sm font-bold text-slate-300 hover:bg-surface-container-highest transition-colors">Cancel</button>
        <button @click="saveRecipients" class="px-4 py-2 rounded text-sm font-bold bg-primary text-on-primary hover:brightness-110 transition-colors shadow-lg shadow-primary/20">Set</button>
      </div>
    </div>
  </div>
</template>
