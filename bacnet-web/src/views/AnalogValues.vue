<script setup>
import { useMqtt } from '../composables/useMqtt'
import { computed, onMounted, onUnmounted, ref } from 'vue'

const { bacnetData, isConnected, subscribeToType, unsubscribeFromType, writeValue } = useMqtt()
const objects = computed(() => {
  const data = bacnetData.AV || []
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

onMounted(() => subscribeToType('AV'))
onUnmounted(() => unsubscribeFromType('AV'))

// Control Modal State
const showModal = ref(false)
const selectedObject = ref(null)
const controlValue = ref('')
const controlPriority = ref(8)

const openControlModal = (obj) => {
  selectedObject.value = obj
  controlValue.value = obj.pv
  controlPriority.value = obj.pri || 8
  showModal.value = true
}

const submitControl = () => {
  if (!selectedObject.value) return
  let parsedValue = parseFloat(controlValue.value)
  if (isNaN(parsedValue)) {
    alert("Invalid value")
    return
  }
  
  const [type, idNum] = selectedObject.value.id.split(':')
  writeValue(type, idNum, parsedValue, parseInt(controlPriority.value, 10))
  
  showModal.value = false
}

const closeModal = () => {
  showModal.value = false
  selectedObject.value = null
}
</script>
<template>
  <div class="flex flex-col h-full bg-[#060e20]">
    <!-- Header / Breadcrumbs -->
    <div class="px-6 py-2 flex items-center justify-between border-b border-outline-variant/10 bg-surface-container-low/30">
      <div class="flex items-center gap-4">
        <h1 class="headline-font text-2xl font-bold text-on-surface tracking-tight">Analog Values</h1>
        <span class="text-sm text-slate-500 font-medium">{{ objects.length }} objects discovered</span>
      </div>
      
    </div>
    <!-- Table Container -->
    <div class="flex-1 overflow-auto no-scrollbar">
      <table class="w-full text-left border-collapse high-density-table">
        <thead class="sticky top-0 z-20 bg-surface-container-high shadow-md">
          <tr>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-6">Instance ID</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest">PORT/MOD/CH</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest  ">Object Name</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest text-right ">Present Value</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest text-center w-12">Pri</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest text-center ">Units</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest text-center ">Status</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest  ">Reliability</th>
            <th class="w-10"></th>
          </tr>
        </thead>
        <tbody class="divide-y divide-outline-variant/5">
          <tr v-if="objects.length === 0">
            <td colspan="9" class="text-center py-8 text-slate-500">
              <div v-if="!isConnected" class="flex items-center justify-center gap-2">
                <span class="material-symbols-outlined animate-spin">sync</span>
                Connecting to MQTT Broker...
              </div>
              <div v-else>No Analog Value objects found.</div>
            </td>
          </tr>
          <tr v-for="obj in paginatedObjects" :key="obj.id" @click="openControlModal(obj)" class="group hover:bg-surface-bright/50 transition-colors cursor-pointer" :class="{ 'bg-error/5': obj.sts === 'Alarm', 'bg-tertiary/5': obj.sts === 'Fault' }">
            <td class="text-sm text-slate-400 font-mono py-1 pl-4">{{ obj.id }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4">{{ obj.port }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4">{{ obj.name }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4">{{ obj.pv }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4">{{ obj.pri || 16 }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4">{{ obj.units }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4">
              <span class="inline-flex px-1.5 py-0.5 rounded text-sm font-bold uppercase" :class="{
                'bg-primary/10 text-primary': obj.sts === 'Normal',
                'bg-error/10 text-error': obj.sts === 'Alarm',
                'bg-tertiary/10 text-tertiary': obj.sts === 'Fault'
              }">{{ obj.sts }}</span>
            </td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4">{{ obj.rel }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4"><button class="material-symbols-outlined text-sm text-slate-600 group-hover:text-primary">more_vert</button></td>
          </tr>
        </tbody>
      </table>
    </div>
    <!-- Status Footer -->
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

    <!-- Control Modal -->
    <div v-if="showModal" class="fixed inset-0 z-50 flex items-center justify-center bg-black/60 backdrop-blur-sm">
      <div class="bg-surface-container-high rounded-lg shadow-2xl border border-outline-variant/20 w-full max-w-md overflow-hidden">
        <div class="px-6 py-4 border-b border-outline-variant/10 flex justify-between items-center bg-surface-container-low">
          <h2 class="text-lg font-bold text-on-surface">Control Object</h2>
          <button @click="closeModal" class="text-slate-400 hover:text-white transition-colors">
            <span class="material-symbols-outlined">close</span>
          </button>
        </div>
        <div class="p-6 space-y-4">
          <div>
            <label class="block text-sm font-bold text-slate-400 mb-1">Object</label>
            <div class="text-on-surface font-mono bg-surface-container p-2 rounded border border-outline-variant/10">{{ selectedObject?.id }} - {{ selectedObject?.name }}</div>
          </div>
          <div>
            <label class="block text-sm font-bold text-slate-400 mb-1">Present Value</label>
            <input type="number" step="any" v-model="controlValue" class="w-full bg-surface-container-highest border border-outline-variant/20 rounded px-3 py-2 text-on-surface focus:outline-none focus:border-primary transition-colors" />
          </div>
          <div>
            <label class="block text-sm font-bold text-slate-400 mb-1">Priority</label>
            <select v-model="controlPriority" class="w-full bg-surface-container-highest border border-outline-variant/20 rounded px-3 py-2 text-on-surface focus:outline-none focus:border-primary transition-colors appearance-none">
              <option v-for="n in 16" :key="n" :value="n">{{ n }}</option>
            </select>
          </div>
        </div>
        <div class="px-6 py-4 bg-surface-container-low flex justify-end gap-3 border-t border-outline-variant/10">
          <button @click="closeModal" class="px-4 py-2 rounded text-sm font-bold text-slate-300 hover:bg-surface-container-highest transition-colors">Cancel</button>
          <button @click="submitControl" class="px-4 py-2 rounded text-sm font-bold bg-primary text-on-primary hover:brightness-110 transition-colors shadow-lg shadow-primary/20">Apply Command</button>
        </div>
      </div>
    </div>
  </div>
</template>
