<script setup>
import { useMqtt } from '../composables/useMqtt'
import { computed, onMounted, onUnmounted } from 'vue'

const { bacnetData, subscribeToType, unsubscribeFromType } = useMqtt()

onMounted(() => subscribeToType('SYS'))
onUnmounted(() => unsubscribeFromType('SYS'))

const sysData = computed(() => bacnetData.SYS || {
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

const sortedEvents = computed(() => {
  const events = [...(sysData.value.events || [])]
  return events.sort((a, b) => b.timestamp.localeCompare(a.timestamp)).slice(0, 5)
})
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
    <div class="bg-surface-container-low px-4 py-2 rounded flex flex-col items-end border-l-4 border-primary">
    <span class="text-primary font-space text-xl font-bold">{{ sysData.bacnetInstance }}</span>
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
    <!-- 240-degree circular arc matching radius 54 -->
    <path d="M 17.24,91 A 54,54 0 1,1 110.76,91" fill="transparent" stroke="var(--color-outline-variant, #2b4680)" stroke-width="8" stroke-linecap="round" class="text-surface-container-highest" />
    
    <!-- Gauge Ticks -->
    <line x1="17.24" y1="91" x2="24.2" y2="87" stroke="#8f9fb7" stroke-width="2"/>
    <line x1="64" y1="10" x2="64" y2="18" stroke="#8f9fb7" stroke-width="2"/>
    <line x1="110.76" y1="91" x2="103.8" y2="87" stroke="#8f9fb7" stroke-width="2"/>
    
    <!-- Gauge Numbers inside the dial -->
    <text x="34" y="86" fill="#8f9fb7" font-size="9" text-anchor="middle" font-family="Space Grotesk" class="font-bold opacity-60">0</text>
    <text x="64" y="32" fill="#8f9fb7" font-size="9" text-anchor="middle" font-family="Space Grotesk" class="font-bold opacity-60">50</text>
    <text x="94" y="86" fill="#8f9fb7" font-size="9" text-anchor="middle" font-family="Space Grotesk" class="font-bold opacity-60">100</text>
    
    <!-- Rotating needle pointing to temp. Range 0 to 100 -> angle -120deg to 120deg (240deg total). Tip is at y=24 (radius 40) for a cleaner sweep. -->
    <polygon points="61,64 64,24 67,64" fill="#ee7d77" :style="{ transform: `rotate(${-120 + (sysData.cpuTemp || 0) * 2.4}deg)`, transformOrigin: '64px 64px' }" class="transition-transform duration-500 ease-out" />
    
    <!-- Center Hub -->
    <circle cx="64" cy="64" r="8" fill="#dee5ff" stroke="#060e20" stroke-width="2"/>
    <circle cx="64" cy="64" r="3" fill="#ee7d77"/>
    </svg>
    <div class="absolute inset-x-0 bottom-[-2px] flex flex-col items-center">
    <span class="text-[9px] sm:text-[10px] text-secondary-dim font-bold uppercase tracking-wider leading-none mb-0.5">Temp</span>
    <span class="text-base sm:text-lg font-bold font-space text-on-surface leading-none pb-1">{{ sysData.cpuTemp ? sysData.cpuTemp.toFixed(1) : '-' }}°C</span>
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
          <span class="font-mono text-sm text-on-surface">{{ sysData.systemTime }}</span>
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
      </div>
    </div>
    <!-- Recent Events Log (Row 3) -->
    <div class="col-span-12 bg-surface-container-low overflow-hidden">
    <div class="px-4 py-3 border-b border-outline-variant/10 flex justify-between items-center">
    <h3 class="font-space text-lg font-bold uppercase tracking-wider text-on-surface">Recent Events Log</h3>
    <div class="flex gap-4">
    <button class="text-xs font-bold text-secondary-dim flex items-center gap-2 px-3 py-1 bg-surface-container-highest rounded border border-outline-variant/20 hover:text-on-surface transition-colors">
    <span class="material-symbols-outlined text-sm">filter_list</span> Filter
                            </button>
    <button class="text-xs font-bold text-secondary-dim flex items-center gap-2 px-3 py-1 bg-surface-container-highest rounded border border-outline-variant/20 hover:text-on-surface transition-colors">
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
</template>
