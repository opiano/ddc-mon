<script setup>
import { ref } from 'vue'
import TopAppBar from '../components/TopAppBar.vue'
import SideNavBar from '../components/SideNavBar.vue'

const isMobileSidebarOpen = ref(false)

const toggleSidebar = () => {
  isMobileSidebarOpen.value = !isMobileSidebarOpen.value
}
</script>

<template>
  <div class="flex flex-col h-screen h-[100dvh] overflow-hidden">
    <TopAppBar @toggle-sidebar="toggleSidebar" />
    <div class="flex flex-1 overflow-hidden relative">
      <!-- Mobile Overlay -->
      <div v-if="isMobileSidebarOpen" 
           @click="isMobileSidebarOpen = false" 
           class="fixed inset-0 bg-black/60 z-40 md:hidden backdrop-blur-sm transition-opacity">
      </div>
      
      <!-- Sidebar -->
      <div :class="[
        'absolute inset-y-0 left-0 transform transition-transform duration-300 ease-in-out z-50 md:relative md:transform-none h-full',
        isMobileSidebarOpen ? 'translate-x-0' : '-translate-x-full'
      ]">
        <SideNavBar @close="isMobileSidebarOpen = false" />
      </div>

      <main class="flex-1 flex flex-col bg-[#060e20] overflow-hidden relative w-full">
        <router-view />
      </main>
    </div>
  </div>
</template>
