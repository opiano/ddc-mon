<script setup>
import { useMqtt } from '../composables/useMqtt'
import { computed, onMounted, onUnmounted, ref } from 'vue'

const { bacnetData, isConnected, subscribeToType, unsubscribeFromType } = useMqtt()
const objects = computed(() => {
  const data = bacnetData.BI || []
  return [...data].sort((a, b) => {
    const idA = parseInt(a.id.split(':')[1], 10)
    const idB = parseInt(b.id.split(':')[1], 10)
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
  subscribeToType('BI')
})

onUnmounted(() => {
  unsubscribeFromType('BI')
})
</script>
<template>
  <div class="flex flex-col h-full bg-[#060e20]">
    <!-- Header / Breadcrumbs -->
    <div class="px-6 py-2 flex items-center justify-between border-b border-outline-variant/10 bg-surface-container-low/30">
      <div class="flex items-center gap-4">
        <h1 class="headline-font text-2xl font-bold text-on-surface tracking-tight">Binary Inputs</h1>
        <span class="text-sm text-slate-500 font-medium">{{ objects.length }} objects discovered</span>
      </div>
      
    </div>
    <!-- Table Container -->
    <div class="flex-1 overflow-auto no-scrollbar">
      <table class="w-full text-left border-collapse high-density-table">
        <thead class="sticky top-0 z-20 bg-surface-container-high shadow-md">
          <tr>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-6">Instance ID</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest">Port/Module</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest  ">Object Name</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest text-right ">Present Value</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest text-center ">Status</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest  ">Reliability</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest text-center ">OOS</th>
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
              <div v-else>No Binary Input objects found.</div>
            </td>
          </tr>
          <tr v-for="obj in paginatedObjects" :key="obj.id" class="group hover:bg-surface-bright/50 transition-colors cursor-pointer" :class="{ 'bg-error-container/5': obj.sts === 'FAULT' || obj.sts === 'Fault', 'bg-tertiary/5': obj.sts === 'OFFLINE' }">
            <td class="text-sm text-slate-400 font-mono py-1 pl-4">{{ obj.id }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4">{{ obj.port }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4">{{ obj.name }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4">
              <div class="inline-flex items-center gap-2 px-2.5 py-1 rounded text-sm font-bold uppercase"
                   :class="obj.pv === 'Active' || obj.pv === true ? 'bg-emerald-500/10 border border-emerald-500/20 text-emerald-400' : 'bg-slate-500/10 border border-slate-500/20 text-slate-500'">
                <span class="w-1.5 h-1.5 rounded-full" :class="obj.pv === 'Active' || obj.pv === true ? 'bg-emerald-500' : 'bg-slate-600'"></span>
                {{ obj.pv === true ? 'Active' : (obj.pv === false ? 'Inactive' : obj.pv) }}
              </div>
            </td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4">{{ obj.sts ? obj.sts.toUpperCase() : 'NORMAL' }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4">{{ obj.rel }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4">
              <span v-if="obj.oos" class="text-sm font-bold text-primary bg-primary/10 px-2 py-0.5 rounded">TRUE</span>
              <span v-else class="text-sm font-bold text-slate-600 bg-slate-500/10 px-2 py-0.5 rounded">FALSE</span>
            </td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4"><button class="material-symbols-outlined text-sm text-slate-600 group-hover:text-primary">more_vert</button></td>
          </tr>
        </tbody>
      </table>
    </div>
    <!-- Status Footer -->
    <footer class="px-6 py-2 bg-surface-container-high border-t border-outline-variant/10 flex items-center justify-between text-sm font-bold text-slate-500 uppercase tracking-widest">
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
