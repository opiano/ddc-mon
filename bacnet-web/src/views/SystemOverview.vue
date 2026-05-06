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
  memoryUsage: 0
})
</script>
<template>
  <div class="p-8 flex-1 overflow-auto bg-surface-dim">
    <!-- System Health Header -->
    <header class="flex flex-col md:flex-row md:items-center justify-between mb-8 gap-4">
    <div>
    <h1 class="text-4xl font-bold font-space tracking-tight text-on-surface mb-1">System Overview</h1>
    <p class="text-secondary font-label text-sm uppercase tracking-widest">REAL-TIME MONITOR</p>
    </div>
    <div class="flex gap-4">
    <div class="bg-surface-container-low px-6 py-4 rounded flex flex-col items-end border-l-4 border-primary">
    <span class="text-primary font-space text-2xl font-bold">{{ sysData.bacnetInstance }}</span>
    <span class="text-xs text-secondary-dim font-label uppercase">BACnet Instance</span>
    </div>
    <div class="bg-surface-container-low px-6 py-4 rounded flex flex-col items-end border-l-4 border-emerald-500">
    <span class="text-emerald-400 font-space text-2xl font-bold">{{ sysData.systemStatus }}</span>
    <span class="text-xs text-secondary-dim font-label uppercase">System Status</span>
    </div>
    </div>
    </header>
    <!-- Bento Grid Main Content -->
    <div class="grid grid-cols-12 gap-6">
    <!-- Metric Cards (Row 1) -->
    <div class="col-span-12 md:col-span-4 bg-surface-container-high p-6 border-l-2 border-outline-variant/20 hover:bg-surface-bright transition-colors duration-300">
    <div class="flex justify-between items-start mb-4">
    <span class="material-symbols-outlined text-primary text-3xl">hub</span>
    <span class="text-xs font-bold text-primary px-2 py-1 bg-primary/10 rounded">LIVE</span>
    </div>
    <div class="text-4xl font-bold font-space text-on-surface">{{ sysData.totalObjects }}</div>
    <div class="text-sm text-secondary uppercase tracking-tight">Total Objects</div>
    </div>
    <div class="col-span-12 md:col-span-4 bg-surface-container-high p-6 border-l-2 border-outline-variant/20 hover:bg-surface-bright transition-colors duration-300">
    <div class="flex justify-between items-start mb-4">
    <span class="material-symbols-outlined text-primary text-3xl">router</span>
    </div>
    <div class="text-4xl font-bold font-space text-on-surface">{{ sysData.modules }}</div>
    <div class="text-sm text-secondary uppercase tracking-tight">Modules</div>
    </div>
    <div class="col-span-12 md:col-span-4 bg-surface-container-high p-6 border-l-2 border-error/50 hover:bg-error-container/20 transition-colors duration-300">
    <div class="flex justify-between items-start mb-4">
    <span class="material-symbols-outlined text-error text-3xl">warning</span>
    <span class="text-xs font-bold text-error px-2 py-1 bg-error/10 rounded">CRITICAL</span>
    </div>
    <div class="text-4xl font-bold font-space text-error">{{ sysData.activeAlarms }}</div>
    <div class="text-sm text-on-error-container uppercase tracking-tight">Active Alarms</div>
    </div>
    <!-- Alarm Distribution & Resources (Row 2) -->
    <div class="col-span-12 lg:col-span-5 bg-surface-container-low p-8">
    <div class="flex justify-between items-center mb-8">
    <h3 class="font-space text-lg font-bold uppercase tracking-wider text-on-surface">System Resources</h3>
    </div>
    <div class="grid grid-cols-2 gap-8 h-full pb-8">
    <!-- Custom Gauge Implementation -->
    <div class="flex flex-col items-center justify-center relative">
    <svg class="w-32 h-32 transform -rotate-90">
    <circle class="text-surface-container-highest" cx="64" cy="64" fill="transparent" r="54" stroke="currentColor" stroke-width="8"></circle>
    <circle class="text-primary" cx="64" cy="64" fill="transparent" r="54" stroke="currentColor" stroke-dasharray="339.29" :stroke-dashoffset="339.29 - (339.29 * sysData.cpuUsage / 100)" stroke-width="8"></circle>
    </svg>
    <div class="absolute inset-0 flex flex-col items-center justify-center mt-[-4px]">
    <span class="text-2xl font-bold font-space">{{ sysData.cpuUsage }}%</span>
    <span class="text-[10px] text-secondary-dim font-bold uppercase">CPU</span>
    </div>
    </div>
    <div class="flex flex-col items-center justify-center relative">
    <svg class="w-32 h-32 transform -rotate-90">
    <circle class="text-surface-container-highest" cx="64" cy="64" fill="transparent" r="54" stroke="currentColor" stroke-width="8"></circle>
    <circle class="text-primary" cx="64" cy="64" fill="transparent" r="54" stroke="currentColor" stroke-dasharray="339.29" :stroke-dashoffset="339.29 - (339.29 * sysData.memoryUsage / 100)" stroke-width="8"></circle>
    </svg>
    <div class="absolute inset-0 flex flex-col items-center justify-center mt-[-4px]">
    <span class="text-2xl font-bold font-space">{{ sysData.memoryUsage }}%</span>
    <span class="text-[10px] text-secondary-dim font-bold uppercase">Memory</span>
    </div>
    </div>
    </div>
    </div>
    <div class="col-span-12 lg:col-span-7 bg-surface-container-low p-8">
    <div class="flex justify-between items-center mb-8">
    <h3 class="font-space text-lg font-bold uppercase tracking-wider text-on-surface">Alarm Distribution</h3>
    <span class="material-symbols-outlined text-slate-500">more_vert</span>
    </div>
    <div class="flex flex-col gap-6">
    <div class="space-y-2">
    <div class="flex justify-between text-xs font-label">
    <span class="text-error uppercase">Critical Alarms</span>
    <span class="text-on-surface font-bold">3 / 26 Total</span>
    </div>
    <div class="w-full h-3 bg-surface-container-highest rounded-full overflow-hidden">
    <div class="h-full bg-error" style="width: 12%"></div>
    </div>
    </div>
    <div class="space-y-2">
    <div class="flex justify-between text-xs font-label">
    <span class="text-tertiary uppercase">Warning Thresholds</span>
    <span class="text-on-surface font-bold">8 / 26 Total</span>
    </div>
    <div class="w-full h-3 bg-surface-container-highest rounded-full overflow-hidden">
    <div class="h-full bg-tertiary" style="width: 31%"></div>
    </div>
    </div>
    <div class="space-y-2">
    <div class="flex justify-between text-xs font-label">
    <span class="text-primary uppercase">Information logs</span>
    <span class="text-on-surface font-bold">15 / 26 Total</span>
    </div>
    <div class="w-full h-3 bg-surface-container-highest rounded-full overflow-hidden">
    <div class="h-full bg-primary" style="width: 57%"></div>
    </div>
    </div>
    </div>
    </div>
    <!-- Recent Events Log (Row 3) -->
    <div class="col-span-12 bg-surface-container-low overflow-hidden">
    <div class="px-8 py-6 border-b border-outline-variant/10 flex justify-between items-center">
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
    <th class="text-sm font-bold text-slate-500 uppercase tracking-widest py-2 pl-4">Level</th>
    <th class="text-sm font-bold text-slate-500 uppercase tracking-widest py-2 pl-4">Description</th>
    <th class="text-sm font-bold text-slate-500 uppercase tracking-widest py-2 pl-4">Source</th>
    </tr>
    </thead>
    <tbody class="divide-y divide-outline-variant/5">
    <tr class="hover:bg-surface-container-highest/30 transition-colors">
    <td class="px-8 py-4 text-sm font-medium text-on-surface/80">14:02:45</td>
    <td class="px-8 py-4">
    <span class="text-[10px] font-bold px-2 py-0.5 rounded bg-tertiary/10 text-tertiary border border-tertiary/20 uppercase">Warning</span>
    </td>
    <td class="px-8 py-4 text-sm text-on-surface">High Temp AHU-01</td>
    <td class="px-8 py-4 text-sm font-mono text-primary-dim">Node-04</td>
    </tr>
    <tr class="bg-surface-container-lowest/5 hover:bg-surface-container-highest/30 transition-colors">
    <td class="px-8 py-4 text-sm font-medium text-on-surface/80">13:58:12</td>
    <td class="px-8 py-4">
    <span class="text-[10px] font-bold px-2 py-0.5 rounded bg-primary/10 text-primary border border-primary/20 uppercase">Info</span>
    </td>
    <td class="px-8 py-4 text-sm text-on-surface">System Backup Completed</td>
    <td class="px-8 py-4 text-sm font-mono text-primary-dim">Master-Ctrl</td>
    </tr>
    <tr class="hover:bg-surface-container-highest/30 transition-colors">
    <td class="px-8 py-4 text-sm font-medium text-on-surface/80">13:45:33</td>
    <td class="px-8 py-4">
    <span class="text-[10px] font-bold px-2 py-0.5 rounded bg-error/10 text-error border border-error/20 uppercase">Critical</span>
    </td>
    <td class="px-8 py-4 text-sm text-on-surface">Loss of Communication - Floor 4</td>
    <td class="px-8 py-4 text-sm font-mono text-primary-dim">Router-F4</td>
    </tr>
    <tr class="bg-surface-container-lowest/5 hover:bg-surface-container-highest/30 transition-colors">
    <td class="px-8 py-4 text-sm font-medium text-on-surface/80">13:30:01</td>
    <td class="px-8 py-4">
    <span class="text-[10px] font-bold px-2 py-0.5 rounded bg-primary/10 text-primary border border-primary/20 uppercase">Info</span>
    </td>
    <td class="px-8 py-4 text-sm text-on-surface">Scheduled Lighting Override: Active</td>
    <td class="px-8 py-4 text-sm font-mono text-primary-dim">Node-04</td>
    </tr>
    </tbody>
    </table>
    </div>
    </div>
    </div>
    <!-- Floating Action Button Contextual Suppression check: Dashboard is primary intent, FAB allowed -->
    <button class="fixed bottom-8 right-8 w-14 h-14 rounded bg-gradient-to-br from-primary to-primary-container text-on-primary shadow-2xl flex items-center justify-center hover:scale-110 transition-transform active:scale-95 group">
    <span class="material-symbols-outlined text-3xl">add</span>
    <span class="absolute right-full mr-4 px-3 py-1.5 bg-surface-container-highest text-xs rounded-md opacity-0 group-hover:opacity-100 transition-opacity whitespace-nowrap pointer-events-none border border-outline-variant/20 uppercase tracking-widest font-bold">New Configuration</span>
    </button>
  </div>
</template>
