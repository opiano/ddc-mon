<script setup>
import { useMqtt } from '../composables/useMqtt'
import { computed, onMounted, onUnmounted, ref } from 'vue'

const { bacnetData, isConnected, subscribeToType, unsubscribeFromType, publish } = useMqtt()

const segSuppLabels = {
  0: 'both',
  1: 'transmit',
  2: 'receive',
  3: 'none'
}

const objects = computed(() => {
  const data = bacnetData.DEVLST || []
  return [...data].sort((a, b) => {
    return (a.deviceInstance || 0) - (b.deviceInstance || 0)
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

// Add Device Modal State
const showAddDeviceModal = ref(false)
const newDevice = ref({
  deviceInstance: 100,
  maxApdu: 1476,
  segSupp: 0,
  vendorId: 15,
  devAddr: '1 / 192.168.1.15:47808'
})

const openAddDeviceModal = () => {
  const currentCount = objects.value.length
  newDevice.value = {
    deviceInstance: 100 + (currentCount + 1) * 10,
    maxApdu: 1476,
    segSupp: 0, // Default to 0: both
    vendorId: 15,
    devAddr: `${currentCount + 1} / 192.168.1.${10 + (currentCount + 1)}:47808`
  }
  showAddDeviceModal.value = true
}

const closeAddDeviceModal = () => {
  showAddDeviceModal.value = false
}

const saveNewDevice = () => {
  const d = newDevice.value
  
  // Validate Device Address format: number / string
  const regex = /^\d+\s*\/\s*\S+$/
  if (!regex.test(d.devAddr.trim())) {
    alert("Invalid Device Address format. Please use 'number / string' format (e.g., '1 / 192.168.1.15:47808').")
    return
  }

  // Format: deviceInstance,maxApdu,segSupp,vendorId,devAddr
  const payload = `${d.deviceInstance},${d.maxApdu},${d.segSupp},${d.vendorId},${d.devAddr.trim()}`
  publish('bacnet/command/device/add', payload)
  showAddDeviceModal.value = false
}

onMounted(() => {
  subscribeToType('DEVLST')
})

onUnmounted(() => {
  unsubscribeFromType('DEVLST')
})
</script>

<template>
  <div class="flex flex-col h-full bg-[#060e20]">
    <!-- Header Box -->
    <div class="px-6 py-3 flex flex-col md:flex-row md:items-center justify-between gap-4 border-b border-outline-variant/10 bg-surface-container-low/30">
      <div class="flex items-center gap-4">
        <h1 class="headline-font text-2xl font-bold text-on-surface tracking-tight">Device List</h1>
        <span class="text-sm text-slate-500 font-medium">{{ objects.length }} devices discovered</span>
      </div>
      <!-- Add Device Button -->
      <button @click="openAddDeviceModal" class="px-3 py-1.5 text-xs font-bold text-primary bg-primary/10 hover:bg-primary/20 rounded border border-primary/20 transition-all flex items-center gap-1.5 shadow-sm">
        <span class="material-symbols-outlined text-sm">add</span> Add Device
      </button>
    </div>

    <!-- Table Grid -->
    <div class="flex-1 overflow-auto no-scrollbar">
      <table class="w-full text-left border-collapse high-density-table font-label">
        <thead class="sticky top-0 z-20 bg-surface-container-high shadow-md">
          <tr class="bg-surface-container-high text-xs uppercase tracking-widest text-slate-500 border-b border-outline-variant/10">
            <th class="text-xs font-bold py-3 pl-6 font-space whitespace-nowrap">Device Instance</th>
            <th class="text-xs font-bold py-3 pl-4 font-space whitespace-nowrap">Max APDU</th>
            <th class="text-xs font-bold py-3 pl-4 font-space whitespace-nowrap">Seg Supp</th>
            <th class="text-xs font-bold py-3 pl-4 font-space whitespace-nowrap">SA</th>
            <th class="text-xs font-bold py-3 pl-4 font-space whitespace-nowrap">Max Seg</th>
            <th class="text-xs font-bold py-3 pl-4 font-space whitespace-nowrap">Vendor ID</th>
            <th class="text-xs font-bold py-3 pl-6 font-space whitespace-nowrap">Dev Addr</th>
          </tr>
        </thead>
        <tbody class="divide-y divide-outline-variant/5">
          <!-- Empty State -->
          <tr v-if="objects.length === 0">
            <td colspan="7" class="text-center py-12 text-slate-500 font-mono">
              <div v-if="!isConnected" class="flex items-center justify-center gap-2">
                <span class="material-symbols-outlined animate-spin text-primary">sync</span>
                Connecting to MQTT Broker...
              </div>
              <div v-else class="flex flex-col items-center gap-2">
                <span class="material-symbols-outlined text-4xl text-slate-600 mb-1">devices_other</span>
                No BACnet devices discovered.
              </div>
            </td>
          </tr>
          <!-- Data Rows (Single Line via whitespace-nowrap) -->
          <tr v-for="obj in paginatedObjects" :key="obj.deviceInstance" class="group hover:bg-surface-bright/30 transition-colors">
            <td class="text-sm font-bold text-primary font-mono py-2.5 pl-6 whitespace-nowrap">{{ obj.deviceInstance }}</td>
            <td class="text-sm text-on-surface/90 font-mono py-2.5 pl-4 whitespace-nowrap">
              {{ obj.maxApdu }} <span class="text-slate-500 text-xs">({{ obj.maxApduAccepted }})</span>
            </td>
            <td class="text-sm text-on-surface/80 font-mono py-2.5 pl-4 whitespace-nowrap">
              {{ obj.segSupp }}: {{ segSuppLabels[obj.segSupp] !== undefined ? segSuppLabels[obj.segSupp] : 'unknown' }}
            </td>
            <td class="text-sm text-on-surface/80 font-mono py-2.5 pl-4 whitespace-nowrap">{{ obj.sa }}</td>
            <td class="text-sm text-on-surface/80 font-mono py-2.5 pl-4 whitespace-nowrap">{{ obj.maxSeg }}</td>
            <td class="text-sm text-on-surface/80 font-mono py-2.5 pl-4 whitespace-nowrap">{{ obj.vendorId }}</td>
            <td class="text-sm text-emerald-400 font-mono py-2.5 pl-6 whitespace-nowrap">{{ obj.devAddr }}</td>
          </tr>
        </tbody>
      </table>
    </div>

    <!-- Footer Box -->
    <footer class="px-4 md:px-6 pt-3 pb-6 md:py-2.5 bg-surface-container-high border-t border-outline-variant/10 flex flex-col md:flex-row gap-3 md:gap-0 md:items-center justify-between text-xs font-bold text-slate-500 uppercase tracking-widest font-space">
      <div class="flex items-center gap-4">
        <span>Showing {{ objects.length > 0 ? (currentPage - 1) * itemsPerPage + 1 : 0 }}-{{ Math.min(currentPage * itemsPerPage, objects.length) }} of {{ objects.length }} devices</span>
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

    <!-- Compact Add Device Modal -->
    <div v-if="showAddDeviceModal" class="fixed inset-0 z-[100] flex items-center justify-center bg-black/60 backdrop-blur-sm p-4">
      <div class="bg-surface-container-high rounded-lg shadow-2xl border border-outline-variant/20 w-full max-w-md overflow-hidden transform transition-all animate-in fade-in zoom-in-95 duration-200">
        <div class="px-5 py-3.5 border-b border-outline-variant/10 flex justify-between items-center bg-surface-container-low">
          <h2 class="text-base font-bold text-on-surface flex items-center gap-2 font-space">
            <span class="material-symbols-outlined text-primary text-lg">devices_other</span>
            Add BACnet Device
          </h2>
          <button @click="closeAddDeviceModal" class="text-slate-400 hover:text-white transition-colors">
            <span class="material-symbols-outlined">close</span>
          </button>
        </div>
        <div class="p-5 space-y-3.5">
          <div>
            <label class="block text-xs font-bold text-slate-400 mb-1.5 uppercase tracking-wider font-label">Device Instance</label>
            <input type="number" v-model="newDevice.deviceInstance" class="w-full bg-surface-container-highest border border-outline-variant/20 rounded px-3 py-1.5 text-on-surface font-mono text-sm focus:outline-none focus:border-primary transition-colors" />
          </div>
          <div>
            <label class="block text-xs font-bold text-slate-400 mb-1.5 uppercase tracking-wider font-label">Vendor ID</label>
            <input type="number" v-model="newDevice.vendorId" class="w-full bg-surface-container-highest border border-outline-variant/20 rounded px-3 py-1.5 text-on-surface font-mono text-sm focus:outline-none focus:border-primary transition-colors" />
          </div>
          <div>
            <label class="block text-xs font-bold text-slate-400 mb-1.5 uppercase tracking-wider font-label">Max APDU</label>
            <input type="number" v-model="newDevice.maxApdu" class="w-full bg-surface-container-highest border border-outline-variant/20 rounded px-3 py-1.5 text-on-surface font-mono text-sm focus:outline-none focus:border-primary transition-colors" />
          </div>
          <div>
            <label class="block text-xs font-bold text-slate-400 mb-1.5 uppercase tracking-wider font-label">Seg Supp</label>
            <select v-model="newDevice.segSupp" class="w-full bg-surface-container-highest border border-outline-variant/20 rounded px-3 py-1.5 text-on-surface font-label text-sm focus:outline-none focus:border-primary transition-colors [color-scheme:dark]">
              <option :value="0">0: both</option>
              <option :value="1">1: transmit</option>
              <option :value="2">2: receive</option>
              <option :value="3">3: none</option>
            </select>
          </div>
          <div>
            <label class="block text-xs font-bold text-slate-400 mb-1.5 uppercase tracking-wider font-label">Dev Addr</label>
            <input type="text" v-model="newDevice.devAddr" placeholder="e.g. 1 / 192.168.1.15:47808" class="w-full bg-surface-container-highest border border-outline-variant/20 rounded px-3 py-1.5 text-on-surface font-mono text-sm focus:outline-none focus:border-primary transition-colors" />
          </div>
        </div>
        <div class="px-5 py-3.5 bg-surface-container-low flex justify-end gap-3 border-t border-outline-variant/10">
          <button @click="closeAddDeviceModal" class="px-4 py-2 rounded text-sm font-bold text-slate-300 hover:bg-surface-container-highest transition-colors font-space">Cancel</button>
          <button @click="saveNewDevice" class="px-4 py-2 rounded text-sm font-bold bg-primary text-on-primary hover:brightness-110 transition-colors shadow-lg shadow-primary/20 font-space">Add</button>
        </div>
      </div>
    </div>
  </div>
</template>
