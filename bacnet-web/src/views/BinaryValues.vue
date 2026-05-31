<script setup>
import { useMqtt } from '../composables/useMqtt'
import { computed, onMounted, onUnmounted, ref } from 'vue'

const { bacnetData, isConnected, subscribeToType, unsubscribeFromType, writeValue, writeAlarmConfig } = useMqtt()
const objects = computed(() => {
  const data = bacnetData.BV || []
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

onMounted(() => subscribeToType('BV'))
onUnmounted(() => unsubscribeFromType('BV'))

// Alarm Modal State
const showAlarmModal = ref(false)
const selectedAlarmObject = ref(null)
const alarmEventDetection = ref(true)
const alarmValue = ref('ACTIVE')

const openAlarmModal = (obj) => {
  selectedAlarmObject.value = obj
  alarmEventDetection.value = obj.ed !== 0 && obj.ed !== false
  alarmValue.value = (obj.av === 1 || obj.av === '1' || String(obj.av).toUpperCase() === 'ACTIVE') ? 'ACTIVE' : 'INACTIVE'
  showAlarmModal.value = true
}

const submitAlarmConfig = () => {
  if (!selectedAlarmObject.value) return
  const [type, idNum] = selectedAlarmObject.value.id.split(':')
  const payload = `${alarmEventDetection.value ? 1 : 0},${alarmValue.value === 'ACTIVE' ? 1 : 0}`
  writeAlarmConfig(type, idNum, payload)
  showAlarmModal.value = false
}

const closeAlarmModal = () => {
  showAlarmModal.value = false
  selectedAlarmObject.value = null
}

// Control Modal State
const showModal = ref(false)
const selectedObject = ref(null)
const controlValue = ref(false)
const controlPriority = ref(8)
const isNullChecked = ref(false)

const openControlModal = (obj) => {
  selectedObject.value = obj
  controlValue.value = (obj.pv === 'Active' || obj.pv === true)
  const priVal = parseInt(obj.pri, 10)
  controlPriority.value = (priVal >= 1 && priVal <= 16) ? priVal : 16
  isNullChecked.value = (obj.pv === null || obj.pv === 'Null' || obj.pv === 'null')
  showModal.value = true
}

const submitControl = () => {
  if (!selectedObject.value) return
  const [type, idNum] = selectedObject.value.id.split(':')
  const valueToSend = isNullChecked.value ? 'Null' : (controlValue.value ? 'Active' : 'Inactive')
  writeValue(type, idNum, valueToSend, parseInt(controlPriority.value, 10))
  
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
        <h1 class="headline-font text-2xl font-bold text-on-surface tracking-tight">Binary Values</h1>
        <span class="text-sm text-slate-500 font-medium">{{ objects.length }} objects discovered</span>
      </div>
      
    </div>
    <!-- Table Container -->
    <div class="flex-1 overflow-auto no-scrollbar">
      <table class="w-full text-left border-collapse high-density-table">
        <thead class="sticky top-0 z-20 bg-surface-container-high shadow-md">
          <tr>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-6 w-32">Instance ID</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-4 w-28">PORT/MOD/CH</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-4 w-44">Object Name</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-4 w-32">Present Value</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-4 w-16">Pri</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-4 w-28">Status</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-4 w-36">Event Detection</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-4 w-32">Alarm Value</th>
            <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-4 w-28">Reliability</th>
            <th class="w-10 pl-4"></th>
          </tr>
        </thead>
        <tbody class="divide-y divide-outline-variant/5">
          <tr v-if="objects.length === 0">
            <td colspan="10" class="text-center py-8 text-slate-500">
              <div v-if="!isConnected" class="flex items-center justify-center gap-2">
                <span class="material-symbols-outlined animate-spin">sync</span>
                Connecting to MQTT Broker...
              </div>
              <div v-else>No Binary Value objects found.</div>
            </td>
          </tr>
          <tr v-for="obj in paginatedObjects" :key="obj.id" @click="openControlModal(obj)" class="group hover:bg-surface-bright/50 transition-colors cursor-pointer" :class="{ 'bg-error-container/5': obj.sts === 'FAULT' || obj.sts === 'Fault', 'bg-tertiary/5': obj.sts === 'OFFLINE' }">
            <td class="text-sm text-slate-400 font-mono py-1 pl-6 w-32">{{ obj.id }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4 w-28">{{ obj.port }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4 w-44 truncate max-w-[11rem]">{{ obj.name }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4 w-32">
              <div class="inline-flex items-center px-2.5 py-1 rounded text-sm font-bold uppercase"
                   :class="obj.pv === 'Active' || obj.pv === true ? 'bg-emerald-500/10 border border-emerald-500/20 text-emerald-400' : 'bg-slate-500/10 border border-slate-500/20 text-slate-500'">
                {{ obj.pv === true ? 'Active' : (obj.pv === false ? 'Inactive' : obj.pv) }}
              </div>
            </td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4 w-16">{{ obj.pri || 16 }}</td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4 w-28">
              <span class="inline-flex px-1.5 py-0.5 rounded text-sm font-bold uppercase" :class="{
                'bg-primary/10 text-primary': !obj.sts || obj.sts.toLowerCase() === 'normal',
                'bg-error/10 text-error': obj.sts && (obj.sts.toLowerCase() === 'alarm' || obj.sts.toLowerCase() === 'offnormal' || obj.sts.toLowerCase() === 'off-normal'),
                'bg-tertiary/10 text-tertiary': obj.sts && (obj.sts.toLowerCase() === 'fault' || obj.sts.toLowerCase() === 'offline')
              }">{{ obj.sts || 'Normal' }}</span>
            </td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4 w-36">
              <span v-if="obj.ed === 1 || obj.ed === true" @click.stop="openAlarmModal(obj)" class="cursor-pointer hover:brightness-110 active:scale-95 transition-all inline-flex px-2 py-0.5 rounded text-sm font-bold text-emerald-400 bg-emerald-500/10 border border-emerald-500/20">TRUE</span>
              <span v-else @click.stop="openAlarmModal(obj)" class="cursor-pointer hover:brightness-110 active:scale-95 transition-all inline-flex px-2 py-0.5 rounded text-sm font-bold text-slate-500 bg-slate-500/10 border border-slate-500/20">FALSE</span>
            </td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4 w-32">
              <span @click.stop="openAlarmModal(obj)" class="cursor-pointer hover:brightness-110 active:scale-95 transition-all inline-flex px-1.5 py-0.5 rounded text-sm font-bold uppercase bg-error/10 text-error border border-error/20">
                {{ (obj.av === 1 || obj.av === '1' || String(obj.av).toUpperCase() === 'ACTIVE') ? 'ACTIVE' : 'INACTIVE' }}
              </span>
            </td>
            <td class="text-sm text-slate-400 font-mono py-1 pl-4 w-28">{{ obj.rel }}</td>
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

    <!-- Alarm Configuration Modal -->
    <div v-if="showAlarmModal" class="fixed inset-0 z-50 flex items-center justify-center bg-black/60 backdrop-blur-sm">
      <div class="bg-surface-container-high rounded-lg shadow-2xl border border-outline-variant/20 w-full max-w-md overflow-hidden">
        <div class="px-6 py-4 border-b border-outline-variant/10 flex justify-between items-center bg-surface-container-low">
          <h2 class="text-lg font-bold text-on-surface">Alarm Configuration</h2>
          <button @click="closeAlarmModal" class="text-slate-400 hover:text-white transition-colors">
            <span class="material-symbols-outlined">close</span>
          </button>
        </div>
        <div class="p-6 space-y-4">
          <div>
            <label class="block text-sm font-bold text-slate-400 mb-1">Object</label>
            <div class="text-on-surface font-mono bg-surface-container p-2 rounded border border-outline-variant/10">{{ selectedAlarmObject?.id }} - {{ selectedAlarmObject?.name }}</div>
          </div>
          
          <!-- Event Detection Checkbox -->
          <div class="flex items-center justify-between p-3 bg-surface-container rounded border border-outline-variant/10">
            <span class="text-sm font-bold text-on-surface">Event Detection Enable</span>
            <label class="relative inline-flex items-center cursor-pointer select-none">
              <input type="checkbox" v-model="alarmEventDetection" class="sr-only peer" />
              <div class="w-11 h-6 bg-slate-600 peer-focus:outline-none rounded-full peer peer-checked:after:translate-x-full peer-checked:after:border-white after:content-[''] after:absolute after:top-[2px] after:left-[2px] after:bg-white after:border-slate-300 after:border after:rounded-full after:h-5 after:w-5 after:transition-all peer-checked:bg-primary"></div>
            </label>
          </div>

          <!-- Conditional Inputs (Alarm Value for BV) -->
          <div class="space-y-4 pt-2">
            <div>
              <label class="block text-sm font-bold text-slate-400 mb-1">Alarm Value</label>
              <select v-model="alarmValue" class="w-full bg-surface-container-highest border border-outline-variant/20 rounded px-3 py-2 text-on-surface focus:outline-none focus:border-primary transition-colors appearance-none">
                <option value="ACTIVE">ACTIVE</option>
                <option value="INACTIVE">INACTIVE</option>
              </select>
            </div>
          </div>
        </div>
        <div class="px-6 py-4 bg-surface-container-low flex justify-end gap-3 border-t border-outline-variant/10">
          <button @click="closeAlarmModal" class="px-4 py-2 rounded text-sm font-bold text-slate-300 hover:bg-surface-container-highest transition-colors">Cancel</button>
          <button @click="submitAlarmConfig" class="px-4 py-2 rounded text-sm font-bold bg-primary text-on-primary hover:brightness-110 transition-colors shadow-lg shadow-primary/20">Apply Alarm Settings</button>
        </div>
      </div>
    </div>

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
            <div class="flex justify-between items-center mb-1">
              <label class="block text-sm font-bold text-slate-400">Present Value</label>
              <label class="flex items-center gap-1.5 text-xs text-slate-400 cursor-pointer select-none">
                <input type="checkbox" v-model="isNullChecked" class="rounded bg-[#06122d] border-[#2b4680]/30 text-[#7bd0ff] focus:ring-0 focus:ring-offset-0 w-3.5 h-3.5" />
                <span>NULL</span>
              </label>
            </div>
            <select :disabled="isNullChecked" v-model="controlValue" class="w-full bg-surface-container-highest border border-outline-variant/20 rounded px-3 py-2 text-on-surface focus:outline-none focus:border-primary disabled:opacity-40 disabled:cursor-not-allowed transition-colors appearance-none">
              <option :value="true">Active</option>
              <option :value="false">Inactive</option>
            </select>
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
