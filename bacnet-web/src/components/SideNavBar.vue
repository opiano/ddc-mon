<script setup>
import { useRoute } from 'vue-router'

const route = useRoute()
const emit = defineEmits(['close'])

const navItems = [
  { name: 'System Overview', path: '/', icon: 'dashboard' },
  { name: 'Device List', path: '/device-list', icon: 'devices' },
  { name: 'Analog Inputs', path: '/analog-inputs', icon: 'input' },
  { name: 'Analog Outputs', path: '/analog-outputs', icon: 'output' },
  { name: 'Analog Values', path: '/analog-values', icon: 'data_object' },
  { name: 'Binary Inputs', path: '/binary-inputs', icon: 'toggle_off' },
  { name: 'Binary Outputs', path: '/binary-outputs', icon: 'toggle_on' },
  { name: 'Binary Values', path: '/binary-values', icon: 'power_settings_new' },
  { name: 'Multi-state Values', path: '/multi-state-values', icon: 'layers' },
  { name: 'Calendars', path: '/calendars', icon: 'calendar_month' },
  { name: 'Schedules', path: '/schedules', icon: 'schedule' },
  { name: 'Trend Logs', path: '/trend-logs', icon: 'show_chart' },
  { name: 'FBD', path: '/fbd', icon: 'schema' },
]


</script>

<template>
  <aside class="bg-[#06122d] text-[#7bd0ff] font-['Inter'] text-sm h-full w-64 flex flex-col py-4 border-r border-[#2b4680]/20 flex-shrink-0 z-40">
    <router-link to="/" class="block px-6 mb-6 cursor-pointer hover:opacity-80 transition-opacity">
      <h2 class="text-lg font-bold text-[#7bd0ff]">DDC Controller</h2>
      <p class="text-[10px] uppercase tracking-widest text-slate-500">SMART CONTROL</p>
    </router-link>
    <div class="flex-1 overflow-y-auto no-scrollbar space-y-1 px-3">
      <router-link 
        v-for="item in navItems" :key="item.path" :to="item.path"
        @click="emit('close')"
        :class="['flex items-center gap-3 px-3 py-2 rounded-r-lg transition-all ease-in-out duration-200', 
                 route.path === item.path ? 'bg-[#031d4b] text-[#7bd0ff] font-semibold' : 'text-slate-400 hover:bg-[#00225a]']"
      >
        <span class="material-symbols-outlined text-lg">{{ item.icon }}</span>
        <span>{{ item.name }}</span>
      </router-link>
    </div>
  </aside>
</template>
