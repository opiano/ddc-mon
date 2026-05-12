<script setup>
import { useMqtt } from '../composables/useMqtt'
import { computed, onMounted, onUnmounted, ref } from 'vue'

const { bacnetData, isConnected, subscribeToType, unsubscribeFromType } = useMqtt()
const objects = computed(() => {
  const data = bacnetData.TL || []
  return [...data].sort((a, b) => {
    const idA = parseInt(a.id?.split(':')[1] || 0, 10)
    const idB = parseInt(b.id?.split(':')[1] || 0, 10)
    return idA - idB
  })
})

const currentPage = ref(1)
const itemsPerPage = 20
const totalPages = computed(() => Math.ceil(objects.value.length / itemsPerPage) || 1)

const paginatedObjects = computed(() => {
  const start = (currentPage.value - 1) * itemsPerPage
  const end = start + itemsPerPage
  return objects.value.slice(start, end)
})

const nextPage = () => { if (currentPage.value < totalPages.value) currentPage.value++ }
const prevPage = () => { if (currentPage.value > 1) currentPage.value-- }

onMounted(() => {
  subscribeToType('TL')
})

onUnmounted(() => {
  unsubscribeFromType('TL')
})
</script>
<template>
  <div class="flex flex-col h-full bg-[#060e20]">
    <div class="px-6 py-2 flex items-center justify-between border-b border-outline-variant/10 bg-surface-container-low/30">
      <div class="flex items-center gap-4">
        <h1 class="headline-font text-2xl font-bold text-on-surface tracking-tight">Trend Logs</h1>
        <span class="text-sm text-slate-500 font-medium">{{ objects.length }} objects discovered</span>
      </div>
    </div>
    <div class="flex-1 overflow-auto no-scrollbar">
      <table class="w-full text-left border-collapse high-density-table">
        <thead class="sticky top-0 z-20 bg-surface-container-high shadow-md">
          <tr>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-6">Instance ID</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-4">Object Name</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-4">Enable</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-4">Interval</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-4">Log Ref</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-4">Record Count</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-4">Total Count</th>
            <th class="w-10"></th>
          </tr>
        </thead>
        <tbody class="divide-y divide-outline-variant/5">
          <tr v-if="objects.length === 0">
            <td colspan="8" class="text-center py-8 text-slate-500">
              <div v-if="!isConnected" class="flex items-center justify-center gap-2">
                <span class="material-symbols-outlined animate-spin">sync</span>
                Connecting to MQTT Broker...
              </div>
              <div v-else>No Trend Log objects found.</div>
            </td>
          </tr>
          <tr v-for="obj in paginatedObjects" :key="obj.id" class="group hover:bg-surface-bright/50 transition-colors cursor-pointer">
            <td class="text-sm text-slate-400 font-mono py-1 pl-4">{{ obj.id }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4">{{ obj.name }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4">{{ obj.enable || '-' }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4">{{ obj.interval || '-' }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4">{{ obj.logRef || '-' }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4">{{ obj.rc || '0' }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4">{{ obj.tc || '0' }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4 text-center"><button class="material-symbols-outlined text-sm text-slate-600 group-hover:text-primary">more_vert</button></td>
          </tr>
        </tbody>
      </table>
    </div>
    <footer class="px-4 md:px-6 pt-3 pb-6 md:py-2 bg-surface-container-high border-t border-outline-variant/10 flex flex-col md:flex-row gap-3 md:gap-0 md:items-center justify-between text-sm font-bold text-slate-500 uppercase tracking-widest">
      <div class="flex items-center gap-4">
        <span>Showing {{ objects.length > 0 ? (currentPage - 1) * itemsPerPage + 1 : 0 }}-{{ Math.min(currentPage * itemsPerPage, objects.length) }} of {{ objects.length }} objects</span>
        <div class="flex items-center gap-1.5">
          <span class="h-1.5 w-1.5 rounded-full" :class="isConnected ? 'bg-primary animate-pulse' : 'bg-slate-600'"></span>
          <span>Live Polling: MQTT</span>
        </div>
      </div>
      <div class="flex items-center gap-4">
        <div class="flex items-center gap-2">
          <button @click="prevPage" :disabled="currentPage === 1" class="hover:text-primary disabled:opacity-30 transition-colors">Previous</button>
          <span class="text-primary px-2 border border-outline-variant/20 rounded">{{ String(currentPage).padStart(2, '0') }} / {{ String(totalPages).padStart(2, '0') }}</span>
          <button @click="nextPage" :disabled="currentPage === totalPages" class="hover:text-primary disabled:opacity-30 transition-colors">Next</button>
        </div>
        <div class="h-4 w-px bg-outline-variant/20"></div>
        <span :class="isConnected ? 'text-primary' : 'text-error'">{{ isConnected ? 'Connected' : 'Disconnected' }}</span>
      </div>
    </footer>
  </div>
</template>
