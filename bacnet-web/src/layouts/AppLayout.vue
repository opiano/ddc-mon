<script setup>
import TopAppBar from '../components/TopAppBar.vue'
import SideNavBar from '../components/SideNavBar.vue'
</script>

<template>
  <div class="flex flex-col h-screen overflow-hidden">
    <TopAppBar />
    <div class="flex flex-1 overflow-hidden">
      <SideNavBar />
      <main class="flex-1 flex flex-col bg-[#060e20] overflow-hidden relative">
        <router-view />
      </main>
    </div>
  </div>
</template>
