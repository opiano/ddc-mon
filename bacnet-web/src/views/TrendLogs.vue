<script setup>
import { useMqtt } from '../composables/useMqtt'
import { computed, onMounted, onUnmounted, ref, watch } from 'vue'

const { 
  bacnetData, 
  isConnected, 
  subscribeToType, 
  unsubscribeFromType,
  trendData,
  subscribeToTrend,
  unsubscribeFromTrend
} = useMqtt()

const objects = computed(() => {
  const data = bacnetData.TLOG || []
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

// Selection and Trend Data logic
const selectedObj = ref(null)

const selectObject = (obj) => {
  if (selectedObj.value && selectedObj.value.id !== obj.id) {
    unsubscribeFromTrend(selectedObj.value.id)
  }
  selectedObj.value = obj
  subscribeToTrend(obj.id)
}

const closeDetails = () => {
  if (selectedObj.value) {
    unsubscribeFromTrend(selectedObj.value.id)
  }
  selectedObj.value = null
}

const currentTrendPoints = computed(() => {
  if (!selectedObj.value) return []
  return trendData[selectedObj.value.id] || []
})

// Sort chronologically and limit to 180 points
const points = computed(() => {
  const sorted = [...currentTrendPoints.value].sort((a, b) => a.unixtime - b.unixtime)
  return sorted.slice(-180)
})

const reversePoints = computed(() => {
  return [...points.value].reverse()
})

// Calculations for Statistics
const minVal = computed(() => {
  if (!points.value.length) return 0
  return Math.min(...points.value.map(p => p.value))
})

const maxVal = computed(() => {
  if (!points.value.length) return 0
  return Math.max(...points.value.map(p => p.value))
})

const avgVal = computed(() => {
  if (!points.value.length) return 0
  const sum = points.value.reduce((acc, p) => acc + p.value, 0)
  return sum / points.value.length
})

const curVal = computed(() => {
  if (!points.value.length) return 0
  return points.value[points.value.length - 1].value
})

// SVG Graph Math
const valMin = computed(() => {
  const min = minVal.value
  const max = maxVal.value
  if (min === max) return min - 1
  const margin = (max - min) * 0.1
  return min - margin
})

const valMax = computed(() => {
  const min = minVal.value
  const max = maxVal.value
  if (min === max) return max + 1
  const margin = (max - min) * 0.1
  return max + margin
})

const timeMin = computed(() => points.value.length ? points.value[0].unixtime : 0)
const timeMax = computed(() => points.value.length ? points.value[points.value.length - 1].unixtime : 0)

const svgPoints = computed(() => {
  if (points.value.length < 2) return []
  const tMin = timeMin.value
  const tMax = timeMax.value
  const vMin = valMin.value
  const vMax = valMax.value
  
  const width = 535  // 600 - 45 (left) - 20 (right)
  const height = 200 // 250 - 20 (top) - 30 (bottom)
  const left = 45
  const top = 20

  return points.value.map(p => {
    const x = left + ((p.unixtime - tMin) / (tMax - tMin || 1)) * width
    const y = top + height - ((p.value - vMin) / (vMax - vMin || 1)) * height
    return { x, y, raw: p }
  })
})

const linePath = computed(() => {
  if (!svgPoints.value.length) return ''
  return svgPoints.value.map((p, idx) => `${idx === 0 ? 'M' : 'L'} ${p.x.toFixed(1)} ${p.y.toFixed(1)}`).join(' ')
})

const areaPath = computed(() => {
  if (!svgPoints.value.length) return ''
  const first = svgPoints.value[0]
  const last = svgPoints.value[svgPoints.value.length - 1]
  const bottomY = 220
  return `${linePath.value} L ${last.x.toFixed(1)} ${bottomY} L ${first.x.toFixed(1)} ${bottomY} Z`
})

const gridLines = computed(() => {
  const vMin = valMin.value
  const vMax = valMax.value
  const lines = []
  for (let i = 0; i <= 4; i++) {
    const val = vMin + (i / 4) * (vMax - vMin)
    const y = 220 - (i / 4) * 200
    lines.push({ val, y })
  }
  return lines
})

// Time Formats
const formatTime = (ts) => {
  const d = new Date(ts * 1000)
  return `${String(d.getHours()).padStart(2, '0')}:${String(d.getMinutes()).padStart(2, '0')}:${String(d.getSeconds()).padStart(2, '0')}`
}

const formatDateTime = (ts) => {
  const d = new Date(ts * 1000)
  return `${d.getMonth() + 1}/${d.getDate()} ${String(d.getHours()).padStart(2, '0')}:${String(d.getMinutes()).padStart(2, '0')}:${String(d.getSeconds()).padStart(2, '0')}`
}

// Hover Interaction
const hoverPoint = ref(null)

const handleMouseMove = (event) => {
  if (!svgPoints.value.length) return
  const svg = event.currentTarget
  const rect = svg.getBoundingClientRect()
  const clientX = event.clientX - rect.left
  const svgX = (clientX / rect.width) * 600

  let closest = svgPoints.value[0]
  let minDist = Math.abs(closest.x - svgX)
  for (let i = 1; i < svgPoints.value.length; i++) {
    const dist = Math.abs(svgPoints.value[i].x - svgX)
    if (dist < minDist) {
      minDist = dist
      closest = svgPoints.value[i]
    }
  }
  hoverPoint.value = closest
}

const handleMouseLeave = () => {
  hoverPoint.value = null
}

onMounted(() => {
  subscribeToType('TLOG')
})

onUnmounted(() => {
  unsubscribeFromType('TLOG')
  if (selectedObj.value) {
    unsubscribeFromTrend(selectedObj.value.id)
  }
})
</script>

<template>
  <div class="flex flex-col h-full bg-[#060e20]">
    <!-- Header -->
    <div class="px-6 py-2 flex items-center justify-between border-b border-outline-variant/10 bg-surface-container-low/30">
      <div class="flex items-center gap-4">
        <h1 class="headline-font text-2xl font-bold text-on-surface tracking-tight">Trend Logs</h1>
        <span class="text-sm text-slate-500 font-medium">{{ objects.length }} objects discovered</span>
      </div>
    </div>

    <!-- Main Workspace Layout -->
    <div class="flex-1 flex overflow-hidden">
      <!-- Left Panel: Trend Logs List -->
      <div class="flex-1 flex flex-col min-w-0 overflow-auto no-scrollbar">
        <table class="w-full text-left border-collapse high-density-table">
          <thead class="sticky top-0 z-20 bg-surface-container-high shadow-md">
            <tr>
              <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-6">Instance ID</th>
              <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-4">Object Name</th>
              <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-4">Enable</th>
              <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-4">Interval</th>
              <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-4">Log Ref</th>
              <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pl-4">Record Count</th>
              <th class="text-sm font-bold text-slate-500 uppercase tracking-widest pr-6">Total Count</th>
            </tr>
          </thead>
          <tbody class="divide-y divide-outline-variant/5">
            <tr v-if="objects.length === 0">
              <td colspan="7" class="text-center py-8 text-slate-500">
                <div v-if="!isConnected" class="flex items-center justify-center gap-2">
                  <span class="material-symbols-outlined animate-spin">sync</span>
                  Connecting to MQTT Broker...
                </div>
                <div v-else>No Trend Log objects found.</div>
              </td>
            </tr>
            <tr 
              v-for="obj in paginatedObjects" 
              :key="obj.id" 
              @click="selectObject(obj)"
              :class="[
                'group hover:bg-surface-bright/50 transition-colors cursor-pointer',
                selectedObj?.id === obj.id ? 'bg-surface-bright/70 font-semibold text-primary border-l-2 border-primary' : ''
              ]"
            >
              <td class="text-sm text-slate-400 font-mono py-1.5 pl-6">{{ obj.id }}</td>
              <td class="text-sm text-slate-400 font-mono py-1.5 pl-4">{{ obj.name }}</td>
              <td class="text-sm text-slate-400 font-mono py-1.5 pl-4">{{ obj.enable || '-' }}</td>
              <td class="text-sm text-slate-400 font-mono py-1.5 pl-4">{{ obj.interval || '-' }}s</td>
              <td class="text-sm text-slate-400 font-mono py-1.5 pl-4">{{ obj.logRef || '-' }}</td>
              <td class="text-sm text-slate-400 font-mono py-1.5 pl-4">{{ obj.rc || '0' }}</td>
              <td class="text-sm text-slate-400 font-mono py-1.5 pl-4 pr-6">{{ obj.tc || '0' }}</td>
            </tr>
          </tbody>
        </table>
      </div>

      <!-- Right Panel: Interactive Graph & Details -->
      <div 
        v-if="selectedObj" 
        class="w-full md:w-[480px] lg:w-[560px] border-l border-outline-variant/10 bg-surface-container-low/40 flex flex-col h-full overflow-hidden transition-all duration-300 shadow-2xl"
      >
        <!-- Detail Header -->
        <div class="px-5 py-4 border-b border-outline-variant/10 flex items-center justify-between bg-surface-container-high/40">
          <div class="flex flex-col">
            <span class="text-xs font-bold text-primary uppercase tracking-widest">Trend Log Details</span>
            <span class="text-base font-bold text-on-surface font-headline">{{ selectedObj.name }}</span>
            <span class="text-xs text-slate-500 font-mono">{{ selectedObj.id }}</span>
          </div>
          <button 
            @click="closeDetails" 
            class="h-8 w-8 rounded-full flex items-center justify-center hover:bg-surface-bright text-slate-400 hover:text-on-surface transition-colors"
          >
            <span class="material-symbols-outlined text-lg">close</span>
          </button>
        </div>

        <!-- Detail Content -->
        <div class="flex-1 overflow-auto p-5 space-y-6 no-scrollbar">
          <!-- Graph Container -->
          <div class="space-y-2">
            <span class="text-xs font-bold text-slate-500 uppercase tracking-widest">Real-time Time-Series (Last 180 Points)</span>
            
            <div class="relative w-full h-[250px] bg-surface-container-lowest border border-outline-variant/10 rounded-xl p-2 overflow-hidden shadow-inner">
              <!-- Loading State -->
              <div v-if="points.length === 0" class="absolute inset-0 flex flex-col items-center justify-center gap-3 bg-surface-container-lowest/80 backdrop-blur-sm z-10">
                <span class="material-symbols-outlined animate-spin text-primary text-3xl">sync</span>
                <span class="text-xs font-mono text-slate-400">Requesting trend data via MQTT...</span>
              </div>

              <!-- SVG Chart -->
              <svg 
                v-else
                class="w-full h-full cursor-crosshair overflow-visible"
                viewBox="0 0 600 250"
                @mousemove="handleMouseMove"
                @mouseleave="handleMouseLeave"
              >
                <!-- Area Gradient Defs -->
                <defs>
                  <linearGradient id="chart-gradient" x1="0" y1="0" x2="0" y2="1">
                    <stop offset="0%" stop-color="#7bd0ff" stop-opacity="0.35" />
                    <stop offset="100%" stop-color="#7bd0ff" stop-opacity="0.0" />
                  </linearGradient>
                </defs>

                <!-- Grid lines & Y Axis Labels -->
                <g v-for="line in gridLines" :key="line.y">
                  <line 
                    x1="45" 
                    :y1="line.y" 
                    x2="580" 
                    :y2="line.y" 
                    stroke="rgba(43, 70, 128, 0.15)" 
                    stroke-width="1"
                    :stroke-dasharray="line.y === 220 ? 'none' : '3,3'" 
                  />
                  <text 
                    x="35" 
                    :y="line.y + 4" 
                    fill="#91aaeb" 
                    font-size="9" 
                    text-anchor="end" 
                    font-family="monospace"
                  >
                    {{ line.val.toFixed(1) }}
                  </text>
                </g>

                <!-- SVG Paths -->
                <path :d="areaPath" fill="url(#chart-gradient)" />
                <path 
                  :d="linePath" 
                  fill="none" 
                  stroke="#7bd0ff" 
                  stroke-width="2.5" 
                  stroke-linecap="round" 
                  stroke-linejoin="round" 
                />

                <!-- X Axis Time Labels -->
                <g v-if="points.length >= 2">
                  <text x="45" y="242" fill="#91aaeb" font-size="9" text-anchor="start" font-family="monospace">
                    {{ formatTime(timeMin) }}
                  </text>
                  <text x="312" y="242" fill="#91aaeb" font-size="9" text-anchor="middle" font-family="monospace">
                    {{ formatTime(timeMin + (timeMax - timeMin) / 2) }}
                  </text>
                  <text x="580" y="242" fill="#91aaeb" font-size="9" text-anchor="end" font-family="monospace">
                    {{ formatTime(timeMax) }}
                  </text>
                </g>

                <!-- Crosshair & Hover Tooltip Dot -->
                <g v-if="hoverPoint">
                  <!-- Vertical Crosshair line -->
                  <line 
                    :x1="hoverPoint.x" 
                    y1="20" 
                    :x2="hoverPoint.x" 
                    y2="220" 
                    stroke="#7bd0ff" 
                    stroke-width="1" 
                    stroke-dasharray="2,2" 
                  />
                  <!-- Hover Value Marker -->
                  <circle 
                    :cx="hoverPoint.x" 
                    :cy="hoverPoint.y" 
                    r="5" 
                    fill="#7bd0ff" 
                    stroke="#060e20" 
                    stroke-width="1.5" 
                  />
                  <circle 
                    :cx="hoverPoint.x" 
                    :cy="hoverPoint.y" 
                    r="10" 
                    fill="#7bd0ff" 
                    fill-opacity="0.2" 
                  />
                </g>
              </svg>

              <!-- Tooltip overlay -->
              <div 
                v-if="hoverPoint"
                class="absolute pointer-events-none bg-surface-container-high/95 border border-primary/30 rounded-xl p-2 shadow-2xl text-[10px] font-mono backdrop-blur-md transition-all duration-75 flex flex-col gap-0.5 min-w-[110px]"
                :style="{
                  left: ((hoverPoint.x / 600) * 100) + '%',
                  top: ((hoverPoint.y / 250) * 100) + '%',
                  transform: 'translate(-50%, -115%)'
                }"
              >
                <span class="text-[#91aaeb]">{{ formatDateTime(hoverPoint.raw.unixtime) }}</span>
                <span class="text-primary font-bold text-xs">Value: {{ hoverPoint.raw.value.toFixed(2) }}</span>
              </div>
            </div>
          </div>

          <!-- Statistics Cards Grid -->
          <div class="space-y-2">
            <span class="text-xs font-bold text-slate-500 uppercase tracking-widest">Statistical Summary</span>
            <div class="grid grid-cols-2 gap-3">
              <!-- Current Value -->
              <div class="p-3.5 rounded-xl border border-primary/20 bg-primary/5 flex items-center gap-3">
                <span class="material-symbols-outlined text-primary text-2xl">insights</span>
                <div class="flex flex-col">
                  <span class="text-[10px] font-bold text-[#91aaeb] uppercase tracking-widest">Latest</span>
                  <span class="text-base font-bold text-primary font-mono leading-none mt-1">{{ curVal.toFixed(2) }}</span>
                </div>
              </div>
              <!-- Average -->
              <div class="p-3.5 rounded-xl border border-slate-500/10 bg-surface-container-high/20 flex items-center gap-3">
                <span class="material-symbols-outlined text-slate-400 text-2xl">functions</span>
                <div class="flex flex-col">
                  <span class="text-[10px] font-bold text-[#91aaeb] uppercase tracking-widest">Average</span>
                  <span class="text-base font-bold text-on-surface font-mono leading-none mt-1">{{ avgVal.toFixed(2) }}</span>
                </div>
              </div>
              <!-- Max -->
              <div class="p-3.5 rounded-xl border border-error/20 bg-error/5 flex items-center gap-3">
                <span class="material-symbols-outlined text-error text-2xl">trending_up</span>
                <div class="flex flex-col">
                  <span class="text-[10px] font-bold text-[#91aaeb] uppercase tracking-widest">Maximum</span>
                  <span class="text-base font-bold text-error font-mono leading-none mt-1">{{ maxVal.toFixed(2) }}</span>
                </div>
              </div>
              <!-- Min -->
              <div class="p-3.5 rounded-xl border border-emerald-500/20 bg-emerald-500/5 flex items-center gap-3">
                <span class="material-symbols-outlined text-emerald-400 text-2xl">trending_down</span>
                <div class="flex flex-col">
                  <span class="text-[10px] font-bold text-[#91aaeb] uppercase tracking-widest">Minimum</span>
                  <span class="text-base font-bold text-emerald-400 font-mono leading-none mt-1">{{ minVal.toFixed(2) }}</span>
                </div>
              </div>
            </div>
          </div>

          <!-- History Logs List -->
          <div class="space-y-2 flex flex-col flex-1 min-h-0">
            <span class="text-xs font-bold text-slate-500 uppercase tracking-widest">Chronological Records</span>
            <div class="border border-outline-variant/10 rounded-xl overflow-hidden bg-surface-container-lowest max-h-[220px] overflow-y-auto no-scrollbar">
              <table class="w-full text-left border-collapse high-density-table">
                <thead class="sticky top-0 bg-surface-container-high z-10">
                  <tr>
                    <th class="text-[10px] font-bold text-slate-500 uppercase tracking-widest pl-4">Timestamp</th>
                    <th class="text-[10px] font-bold text-slate-500 uppercase tracking-widest pr-4 text-right">Value</th>
                  </tr>
                </thead>
                <tbody class="divide-y divide-outline-variant/5">
                  <tr v-if="reversePoints.length === 0">
                    <td colspan="2" class="text-center py-4 text-xs text-slate-600">No log entries available.</td>
                  </tr>
                  <tr v-for="pt in reversePoints" :key="pt.unixtime" class="hover:bg-surface-bright/30">
                    <td class="text-xs text-slate-400 font-mono py-1.5 pl-4">{{ formatDateTime(pt.unixtime) }}</td>
                    <td class="text-xs text-primary font-mono py-1.5 pr-4 text-right">{{ pt.value.toFixed(2) }}</td>
                  </tr>
                </tbody>
              </table>
            </div>
          </div>
        </div>
      </div>
    </div>

    <!-- Footer -->
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
