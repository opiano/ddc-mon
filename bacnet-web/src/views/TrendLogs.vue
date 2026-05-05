<script setup>
</script>
<template>
  <div class="flex-1 p-6 overflow-y-auto space-y-6 relative">
    <!-- Header Area -->
    <div class="flex items-end justify-between">
    <div class="space-y-1">
    <span class="text-tertiary font-headline font-bold text-[10px] tracking-[0.2em] uppercase">Network Monitoring</span>
    <h1 class="text-3xl font-bold font-headline tracking-tighter text-on-surface">Trend Log Explorer</h1>
    </div>
    <div class="flex gap-4 items-center">
    <div class="text-right">
    <div class="text-[10px] text-slate-500 font-medium uppercase tracking-wider">Active Logs</div>
    <div class="text-xl font-headline font-bold text-[#7bd0ff]">42 / 42</div>
    </div>
    <div class="text-right">
    <div class="text-[10px] text-slate-500 font-medium uppercase tracking-wider">Total Buffer Usage</div>
    <div class="text-xl font-headline font-bold text-tertiary">68%</div>
    </div>
    </div>
    </div>
    <!-- Main Data Table Container -->
    <div class="bg-surface-container-low rounded shadow-2xl overflow-hidden">
    <table class="w-full text-left border-collapse">
    <thead class="bg-surface-container-high">
    <tr class="text-slate-400 text-[10px] font-bold uppercase tracking-widest">
    <th class="text-sm font-bold text-slate-500 uppercase tracking-widest py-2 pl-4">Trend Name</th>
    <th class="text-sm font-bold text-slate-500 uppercase tracking-widest py-2 pl-4">Log Device</th>
    <th class="text-sm font-bold text-slate-500 uppercase tracking-widest py-2 pl-4">Data Flow</th>
    <th class="text-sm font-bold text-slate-500 uppercase tracking-widest py-2 pl-4">Interval</th>
    <th class="text-sm font-bold text-slate-500 uppercase tracking-widest py-2 pl-4">Buffer Size</th>
    <th class="text-sm font-bold text-slate-500 uppercase tracking-widest py-2 pl-4">Total Samples</th>
    <th class="text-sm font-bold text-slate-500 uppercase tracking-widest py-2 pl-4">Last Value</th>
    <th class="text-sm font-bold text-slate-500 uppercase tracking-widest py-2 pl-4">Last Sample Time</th>
    </tr>
    </thead>
    <tbody class="divide-y divide-outline-variant/5">
    <!-- Row 1 -->
    <tr class="hover:bg-surface-bright/30 transition-colors group">
    <td class="px-6 py-4">
    <div class="flex flex-col">
    <span class="text-sm font-semibold text-primary group-hover:text-primary-dim">Space_Temp_Log</span>
    <span class="text-[10px] text-slate-500 font-medium">BMS_AHU01_TL</span>
    </div>
    </td>
    <td class="px-4 py-4 text-sm text-on-surface-variant font-mono">AHU_Main_Ctrl</td>
    <td class="px-4 py-4">
    <div class="flex justify-center">
    <svg class="w-12 h-6" viewbox="0 0 100 40">
    <path d="M0,35 L10,30 L20,38 L30,20 L40,25 L50,15 L60,18 L70,5 L80,12 L90,10 L100,20" fill="none" stroke="#7bd0ff" stroke-width="2"></path>
    </svg>
    </div>
    </td>
    <td class="px-4 py-4 text-xs font-medium text-on-surface">15 min</td>
    <td class="px-4 py-4 text-xs font-medium text-on-surface">1024</td>
    <td class="px-4 py-4 text-xs font-medium text-on-surface">872</td>
    <td class="px-4 py-4">
    <span class="text-sm font-headline font-bold text-on-surface">22.4</span>
    <span class="text-[10px] text-slate-500 ml-0.5">°C</span>
    </td>
    <td class="px-6 py-4 text-right text-xs font-mono text-slate-500 uppercase tracking-tighter">14:28:45 10-23</td>
    </tr>
    <!-- Row 2 -->
    <tr class="bg-white/5 hover:bg-surface-bright/30 transition-colors group">
    <td class="px-6 py-4">
    <div class="flex flex-col">
    <span class="text-sm font-semibold text-primary group-hover:text-primary-dim">Exhaust_Fan_Power</span>
    <span class="text-[10px] text-slate-500 font-medium">EM_EF_04_TL</span>
    </div>
    </td>
    <td class="px-4 py-4 text-sm text-on-surface-variant font-mono">Power_Meter_S2</td>
    <td class="px-4 py-4">
    <div class="flex justify-center">
    <span class="material-symbols-outlined text-primary-fixed animate-pulse text-lg" style="font-variation-settings: 'FILL' 1;">sync</span>
    </div>
    </td>
    <td class="px-4 py-4 text-xs font-medium text-on-surface">5 min</td>
    <td class="px-4 py-4 text-xs font-medium text-on-surface">2048</td>
    <td class="px-4 py-4 text-xs font-medium text-on-surface">1,940</td>
    <td class="px-4 py-4">
    <span class="text-sm font-headline font-bold text-on-surface">1.2</span>
    <span class="text-[10px] text-slate-500 ml-0.5">kW</span>
    </td>
    <td class="px-6 py-4 text-right text-xs font-mono text-slate-500 uppercase tracking-tighter">14:27:12 10-23</td>
    </tr>
    <!-- Row 3 -->
    <tr class="hover:bg-surface-bright/30 transition-colors group">
    <td class="px-6 py-4">
    <div class="flex flex-col">
    <span class="text-sm font-semibold text-primary group-hover:text-primary-dim">CO2_Level_Floor4</span>
    <span class="text-[10px] text-slate-500 font-medium">AQ_F4_Z02_TL</span>
    </div>
    </td>
    <td class="px-4 py-4 text-sm text-on-surface-variant font-mono">IAQ_Sens_Grid</td>
    <td class="px-4 py-4">
    <div class="flex justify-center">
    <svg class="w-12 h-6" viewbox="0 0 100 40">
    <path d="M0,20 L20,20 L40,25 L60,15 L80,20 L100,20" fill="none" stroke="#ffb148" stroke-width="2"></path>
    </svg>
    </div>
    </td>
    <td class="px-4 py-4 text-xs font-medium text-on-surface">30 min</td>
    <td class="px-4 py-4 text-xs font-medium text-on-surface">512</td>
    <td class="px-4 py-4 text-xs font-medium text-on-surface">512</td>
    <td class="px-4 py-4">
    <span class="text-sm font-headline font-bold text-tertiary">412</span>
    <span class="text-[10px] text-slate-500 ml-0.5">ppm</span>
    </td>
    <td class="px-6 py-4 text-right text-xs font-mono text-slate-500 uppercase tracking-tighter">14:00:00 10-23</td>
    </tr>
    <!-- Row 4 -->
    <tr class="bg-white/5 hover:bg-surface-bright/30 transition-colors group">
    <td class="px-6 py-4">
    <div class="flex flex-col">
    <span class="text-sm font-semibold text-primary group-hover:text-primary-dim">Chiller_Flow_Rate</span>
    <span class="text-[10px] text-slate-500 font-medium">PL_CH_01_TL</span>
    </div>
    </td>
    <td class="px-4 py-4 text-sm text-on-surface-variant font-mono">Plant_Controller</td>
    <td class="px-4 py-4">
    <div class="flex justify-center">
    <span class="material-symbols-outlined text-error text-lg" style="font-variation-settings: 'FILL' 1;">error</span>
    </div>
    </td>
    <td class="px-4 py-4 text-xs font-medium text-on-surface">1 min</td>
    <td class="px-4 py-4 text-xs font-medium text-on-surface">4096</td>
    <td class="px-4 py-4 text-xs font-medium text-on-surface">2,812</td>
    <td class="px-4 py-4">
    <span class="text-sm font-headline font-bold text-on-surface">0.0</span>
    <span class="text-[10px] text-slate-500 ml-0.5">L/s</span>
    </td>
    <td class="px-6 py-4 text-right text-xs font-mono text-slate-500 uppercase tracking-tighter">13:14:22 10-23</td>
    </tr>
    <!-- Row 5 -->
    <tr class="hover:bg-surface-bright/30 transition-colors group">
    <td class="px-6 py-4">
    <div class="flex flex-col">
    <span class="text-sm font-semibold text-primary group-hover:text-primary-dim">Supply_Pressure_Set</span>
    <span class="text-[10px] text-slate-500 font-medium">AHU02_SP_TL</span>
    </div>
    </td>
    <td class="px-4 py-4 text-sm text-on-surface-variant font-mono">AHU_Main_Ctrl</td>
    <td class="px-4 py-4">
    <div class="flex justify-center">
    <svg class="w-12 h-6" viewbox="0 0 100 40">
    <path d="M0,5 L20,35 L40,5 L60,35 L80,5 L100,35" fill="none" stroke="#7bd0ff" stroke-width="2"></path>
    </svg>
    </div>
    </td>
    <td class="px-4 py-4 text-xs font-medium text-on-surface">15 min</td>
    <td class="px-4 py-4 text-xs font-medium text-on-surface">1024</td>
    <td class="px-4 py-4 text-xs font-medium text-on-surface">1,024</td>
    <td class="px-4 py-4">
    <span class="text-sm font-headline font-bold text-on-surface">250</span>
    <span class="text-[10px] text-slate-500 ml-0.5">Pa</span>
    </td>
    <td class="px-6 py-4 text-right text-xs font-mono text-slate-500 uppercase tracking-tighter">14:15:00 10-23</td>
    </tr>
    </tbody>
    </table>
    <!-- Pagination/Footer -->
    <div class="px-6 py-3 bg-surface-container-high flex justify-between items-center">
    <span class="text-[10px] text-slate-500 font-medium uppercase tracking-widest">Showing 1-10 of 42 Logs</span>
    <div class="flex gap-2">
    <button class="p-1.5 rounded hover:bg-[#00225a] text-slate-400 disabled:opacity-30">
    <span class="material-symbols-outlined text-sm">chevron_left</span>
    </button>
    <button class="p-1.5 rounded bg-primary/10 text-primary border border-primary/20 text-[10px] px-3 font-bold">1</button>
    <button class="p-1.5 rounded hover:bg-[#00225a] text-slate-400 text-[10px] px-3 font-bold">2</button>
    <button class="p-1.5 rounded hover:bg-[#00225a] text-slate-400 text-[10px] px-3 font-bold">3</button>
    <button class="p-1.5 rounded hover:bg-[#00225a] text-slate-400">
    <span class="material-symbols-outlined text-sm">chevron_right</span>
    </button>
    </div>
    </div>
    </div>
    <!-- Detail Overlay (Asymmetric Floating Inspector) -->
    <div class="absolute right-6 top-28 w-80 bg-surface-container-highest/90 backdrop-blur-xl border-l border-outline-variant/30 rounded shadow-2xl p-6 hidden lg:block">
    <div class="flex justify-between items-start mb-6">
    <h3 class="font-headline font-bold text-primary tracking-tight">LOG INSPECTOR</h3>
    <span class="material-symbols-outlined text-slate-500 cursor-pointer">close</span>
    </div>
    <div class="space-y-6">
    <div class="space-y-1">
    <label class="text-[10px] text-slate-500 uppercase tracking-widest block">Selected Object</label>
    <p class="text-sm font-semibold">Space_Temp_Log</p>
    </div>
    <div class="space-y-3">
    <div class="flex justify-between items-end border-b border-outline-variant/10 pb-2">
    <span class="text-xs text-slate-400">Type</span>
    <span class="text-xs font-mono text-on-surface">Trend Log</span>
    </div>
    <div class="flex justify-between items-end border-b border-outline-variant/10 pb-2">
    <span class="text-xs text-slate-400">Object ID</span>
    <span class="text-xs font-mono text-on-surface">TL:4</span>
    </div>
    <div class="flex justify-between items-end border-b border-outline-variant/10 pb-2">
    <span class="text-xs text-slate-400">Stop when Full</span>
    <span class="text-xs font-mono text-error">False</span>
    </div>
    <div class="flex justify-between items-end border-b border-outline-variant/10 pb-2">
    <span class="text-xs text-slate-400">Log Type</span>
    <span class="text-xs font-mono text-on-surface">Polled</span>
    </div>
    </div>
    <div class="pt-4">
    <button class="w-full py-2 bg-surface-bright text-primary text-[10px] font-bold rounded border border-primary/20 hover:bg-primary hover:text-on-primary transition-all uppercase tracking-widest">
                  Export CSV Data
                </button>
    </div>
    </div>
    </div>
  </div>
</template>
