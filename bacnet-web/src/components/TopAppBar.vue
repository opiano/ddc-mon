<script setup>
import { ref } from 'vue'
import { useMqtt } from '../composables/useMqtt'

const { brokerUrl, setBrokerUrl } = useMqtt()

const defaultBrokerUrl = `ws://${window.location.hostname}:9001`
const showSettingsModal = ref(false)
const inputBrokerUrl = ref('')

const emit = defineEmits(['toggle-sidebar'])

const openSettings = () => {
  inputBrokerUrl.value = brokerUrl.value || defaultBrokerUrl
  showSettingsModal.value = true
}

const saveSettings = () => {
  if (inputBrokerUrl.value) {
    setBrokerUrl(inputBrokerUrl.value)
  }
  showSettingsModal.value = false
}

const closeSettings = () => {
  showSettingsModal.value = false
}
</script>

<template>
  <header class="bg-[#06122d] text-[#7bd0ff] font-['Space_Grotesk'] tracking-tight flex justify-between items-center w-full px-4 md:px-6 h-12 z-50 border-b border-outline-variant/10">
    <div class="flex items-center gap-2 md:gap-4">
      <button @click="emit('toggle-sidebar')" class="md:hidden material-symbols-outlined p-1 hover:bg-[#00225a] rounded transition-colors text-slate-300">menu</button>
      <span class="text-lg md:text-xl font-bold text-[#7bd0ff] tracking-widest uppercase">SMART BMS</span>
    </div>
    <div class="flex items-center gap-2 md:gap-4">
      <div class="hidden sm:flex relative items-center bg-surface-container-highest/50 px-3 py-1.5 rounded-lg border border-outline-variant/10">
        <span class="material-symbols-outlined text-sm text-secondary-dim mr-2">search</span>
        <input class="bg-transparent border-none focus:ring-0 text-sm w-32 md:w-48 placeholder:text-slate-500 outline-none" placeholder="Search objects..." type="text"/>
      </div>
      <button class="hidden sm:block material-symbols-outlined p-1.5 md:p-2 text-slate-400 hover:text-[#7bd0ff] transition-colors">notifications</button>
      <button @click="openSettings" class="material-symbols-outlined p-1.5 md:p-2 text-slate-400 hover:text-[#7bd0ff] transition-colors">settings</button>
      <img alt="Operator Profile" class="h-7 w-7 md:h-8 md:w-8 rounded-full border border-primary/20" src="https://lh3.googleusercontent.com/aida-public/AB6AXuCSHH8i6XPhsgCO_SDSV4-O9FGL3YEGe9rETnIxWUGfcXPW-gUMjr5MPpsuTyklAvPIJK0dQeDRQ3366kcJ-huGf4b-1fDOmhTCQ2-keZGYJQVYdVgeLeJ4cc4hjdt5JU5vK6VZkJAAzisfoLeRe3PhVhBKb-Tlqi5s3iYfxVMWwqUTqfLUkg_sAoht5sRDB6VTSNiZFI6xk_gqMsQRYahJO3CYF7bu4WzpQLPEV2aVclkQbzbnLlbO6AG3dGGEgszHQxLgqqEIy_Q"/>
    </div>
  </header>

  <!-- Settings Modal -->
  <div v-if="showSettingsModal" class="fixed inset-0 z-[100] flex items-center justify-center bg-black/60 backdrop-blur-sm">
    <div class="bg-surface-container-high rounded-lg shadow-2xl border border-outline-variant/20 w-full max-w-md overflow-hidden">
      <div class="px-6 py-4 border-b border-outline-variant/10 flex justify-between items-center bg-surface-container-low">
        <h2 class="text-lg font-bold text-on-surface flex items-center gap-2">
          <span class="material-symbols-outlined text-primary">settings</span>
          Settings
        </h2>
        <button @click="closeSettings" class="text-slate-400 hover:text-white transition-colors">
          <span class="material-symbols-outlined">close</span>
        </button>
      </div>
      <div class="p-6 space-y-4">
        <div>
          <label class="block text-sm font-bold text-slate-400 mb-2">MQTT Broker URL (WebSocket)</label>
          <input type="text" v-model="inputBrokerUrl" :placeholder="defaultBrokerUrl" class="w-full bg-surface-container-highest border border-outline-variant/20 rounded px-3 py-2 text-on-surface font-mono text-sm focus:outline-none focus:border-primary transition-colors" />
          <p class="text-xs text-slate-500 mt-2">Example: {{ defaultBrokerUrl }}</p>
        </div>
      </div>
      <div class="px-6 py-4 bg-surface-container-low flex justify-end gap-3 border-t border-outline-variant/10">
        <button @click="closeSettings" class="px-4 py-2 rounded text-sm font-bold text-slate-300 hover:bg-surface-container-highest transition-colors">Cancel</button>
        <button @click="saveSettings" class="px-4 py-2 rounded text-sm font-bold bg-primary text-on-primary hover:brightness-110 transition-colors shadow-lg shadow-primary/20">Save & Connect</button>
      </div>
    </div>
  </div>
</template>
