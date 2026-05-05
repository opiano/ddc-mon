<script setup>
</script>
<template>
  <div class="flex flex-row flex-1 h-full overflow-hidden bg-[#060e20]">
    <!-- Content Canvas -->
    <div class="flex-1 overflow-y-auto p-6 space-y-6">
      <!-- Editorial Header Area -->
      <div class="flex justify-between items-end">
      <div>
      <h1 class="headline-font text-3xl font-bold text-on-surface tracking-tight mb-2">Schedules Dashboard</h1>
      <p class="text-on-surface-variant text-sm max-w-xl">Real-time BACnet schedule synchronization across building HVAC and lighting subsystems. Monitoring active occupancy overrides and next event transitions.</p>
      </div>
      <div class="flex gap-3">
      <div class="text-right">
      <span class="text-[10px] text-slate-500 uppercase font-bold tracking-widest block">Active Schedules</span>
      <span class="headline-font text-2xl font-bold text-primary">24 / 28</span>
      </div>
      <div class="text-right ml-8">
      <span class="text-[10px] text-slate-500 uppercase font-bold tracking-widest block">Next Transition</span>
      <span class="headline-font text-2xl font-bold text-tertiary">14:00</span>
      </div>
      </div>
      </div>
      <!-- Bento-style Grid Section (Schedules Table) -->
      <div class="bg-surface-container-low rounded shadow-xl border border-outline-variant/10 overflow-hidden">
      <div class="p-4 flex justify-between items-center bg-surface-container-high/50">
      <div class="flex gap-4">
      <div class="relative">
      <span class="material-symbols-outlined absolute left-3 top-1/2 -translate-y-1/2 text-slate-400 text-sm">search</span>
      <input class="bg-surface-container-lowest border-none text-xs rounded pl-9 pr-4 py-2 w-64 focus:ring-1 focus:ring-primary placeholder:text-slate-600" placeholder="Filter schedules..." type="text"/>
      </div>
      <select class="bg-surface-container-lowest border-none text-xs rounded px-4 py-2 focus:ring-1 focus:ring-primary text-slate-300">
      <option>All Effective Periods</option>
      <option>Active Today</option>
      <option>Exceptions Only</option>
      </select>
      </div>
      <button class="text-xs font-bold text-primary flex items-center gap-2 hover:underline">
      <span class="material-symbols-outlined text-sm">download</span>
                  EXPORT LOGS
                </button>
      </div>
      <!-- High Density Data Table -->
      <div class="overflow-x-auto">
      <table class="w-full text-left border-collapse">
      <thead>
      <tr class="bg-surface-container-low text-on-surface-variant uppercase text-[10px] font-bold tracking-wider">
      <th class="text-sm font-bold text-slate-500 uppercase tracking-widest py-2 pl-4">Schedule Name</th>
      <th class="text-sm font-bold text-slate-500 uppercase tracking-widest py-2 pl-4">Instance ID</th>
      <th class="text-sm font-bold text-slate-500 uppercase tracking-widest py-2 pl-4">Effective Period</th>
      <th class="text-sm font-bold text-slate-500 uppercase tracking-widest py-2 pl-4">Weekly Schedule</th>
      <th class="text-sm font-bold text-slate-500 uppercase tracking-widest py-2 pl-4">Next Change</th>
      <th class="text-sm font-bold text-slate-500 uppercase tracking-widest py-2 pl-4">Current Value</th>
      </tr>
      </thead>
      <tbody class="divide-y divide-outline-variant/5">
      <!-- Row 1 -->
      <tr class="hover:bg-surface-bright/20 transition-colors group">
      <td class="px-6 py-4">
      <div class="flex items-center gap-3">
      <div class="w-2 h-2 rounded-full bg-primary shadow-[0_0_8px_rgba(123,208,255,0.6)]"></div>
      <span class="text-sm font-semibold text-on-surface">Office_Lighting_Sch</span>
      </div>
      </td>
      <td class="px-6 py-4 text-xs font-mono text-secondary-dim">SCH-2104</td>
      <td class="px-6 py-4 text-xs text-on-surface-variant">01 Jan 2024 - 31 Dec 2024</td>
      <td class="px-6 py-4">
      <span class="text-[11px] bg-surface-container-high px-2 py-1 rounded text-slate-300">M-F 08:00-18:00</span>
      </td>
      <td class="px-6 py-4">
      <div class="flex items-center gap-2">
      <span class="material-symbols-outlined text-xs text-tertiary">schedule</span>
      <span class="text-xs text-tertiary font-medium">02h 14m</span>
      </div>
      </td>
      <td class="px-6 py-4 text-right">
      <span class="text-xs font-bold text-primary bg-primary/10 px-3 py-1 rounded border border-primary/20 uppercase">Active (1)</span>
      </td>
      </tr>
      <!-- Row 2 -->
      <tr class="hover:bg-surface-bright/20 transition-colors bg-surface-container-lowest/5">
      <td class="px-6 py-4">
      <div class="flex items-center gap-3">
      <div class="w-2 h-2 rounded-full bg-primary shadow-[0_0_8px_rgba(123,208,255,0.6)]"></div>
      <span class="text-sm font-semibold text-on-surface">AHU_Main_Occupancy</span>
      </div>
      </td>
      <td class="px-6 py-4 text-xs font-mono text-secondary-dim">SCH-1002</td>
      <td class="px-6 py-4 text-xs text-on-surface-variant">Permanent</td>
      <td class="px-6 py-4">
      <span class="text-[11px] bg-surface-container-high px-2 py-1 rounded text-slate-300">M-S 06:00-22:00</span>
      </td>
      <td class="px-6 py-4">
      <div class="flex items-center gap-2">
      <span class="material-symbols-outlined text-xs text-tertiary">schedule</span>
      <span class="text-xs text-tertiary font-medium">06h 14m</span>
      </div>
      </td>
      <td class="px-6 py-4 text-right">
      <span class="text-xs font-bold text-primary bg-primary/10 px-3 py-1 rounded border border-primary/20 uppercase">Occupied</span>
      </td>
      </tr>
      <!-- Row 3 -->
      <tr class="hover:bg-surface-bright/20 transition-colors">
      <td class="px-6 py-4">
      <div class="flex items-center gap-3">
      <div class="w-2 h-2 rounded-full bg-slate-600"></div>
      <span class="text-sm font-semibold text-on-surface">DataCenter_Cooling_Override</span>
      </div>
      </td>
      <td class="px-6 py-4 text-xs font-mono text-secondary-dim">SCH-4401</td>
      <td class="px-6 py-4 text-xs text-on-surface-variant">Seasonal (Summer)</td>
      <td class="px-6 py-4">
      <span class="text-[11px] bg-surface-container-high px-2 py-1 rounded text-slate-300">24/7 Priority-1</span>
      </td>
      <td class="px-6 py-4">
      <div class="flex items-center gap-2">
      <span class="text-xs text-slate-500 font-medium">No Transition</span>
      </div>
      </td>
      <td class="px-6 py-4 text-right">
      <span class="text-xs font-bold text-slate-500 bg-slate-500/10 px-3 py-1 rounded border border-slate-500/20 uppercase">Inactive</span>
      </td>
      </tr>
      <!-- Row 4 -->
      <tr class="hover:bg-surface-bright/20 transition-colors bg-surface-container-lowest/5">
      <td class="px-6 py-4">
      <div class="flex items-center gap-3">
      <div class="w-2 h-2 rounded-full bg-error shadow-[0_0_8px_rgba(238,125,119,0.6)]"></div>
      <span class="text-sm font-semibold text-on-surface">External_Security_Lights</span>
      </div>
      </td>
      <td class="px-6 py-4 text-xs font-mono text-secondary-dim">SCH-0599</td>
      <td class="px-6 py-4 text-xs text-on-surface-variant">01 Jan 2024 - 31 Dec 2024</td>
      <td class="px-6 py-4">
      <span class="text-[11px] bg-surface-container-high px-2 py-1 rounded text-slate-300">Dusk-to-Dawn</span>
      </td>
      <td class="px-6 py-4">
      <div class="flex items-center gap-2">
      <span class="material-symbols-outlined text-xs text-tertiary">schedule</span>
      <span class="text-xs text-tertiary font-medium">04h 52m</span>
      </div>
      </td>
      <td class="px-6 py-4 text-right">
      <span class="text-xs font-bold text-error bg-error/10 px-3 py-1 rounded border border-error/20 uppercase">Fault State</span>
      </td>
      </tr>
      <!-- Row 5 -->
      <tr class="hover:bg-surface-bright/20 transition-colors">
      <td class="px-6 py-4">
      <div class="flex items-center gap-3">
      <div class="w-2 h-2 rounded-full bg-primary shadow-[0_0_8px_rgba(123,208,255,0.6)]"></div>
      <span class="text-sm font-semibold text-on-surface">Gym_HVAC_Schedules</span>
      </div>
      </td>
      <td class="px-6 py-4 text-xs font-mono text-secondary-dim">SCH-9221</td>
      <td class="px-6 py-4 text-xs text-on-surface-variant">Permament</td>
      <td class="px-6 py-4">
      <span class="text-[11px] bg-surface-container-high px-2 py-1 rounded text-slate-300">M-S 05:00-23:00</span>
      </td>
      <td class="px-6 py-4">
      <div class="flex items-center gap-2">
      <span class="material-symbols-outlined text-xs text-tertiary">schedule</span>
      <span class="text-xs text-tertiary font-medium">07h 14m</span>
      </div>
      </td>
      <td class="px-6 py-4 text-right">
      <span class="text-xs font-bold text-primary bg-primary/10 px-3 py-1 rounded border border-primary/20 uppercase">Occupied</span>
      </td>
      </tr>
      </tbody>
      </table>
      </div>
      </div>
      <!-- Detail/Inspector Layout -->
      <div class="grid grid-cols-1 lg:grid-cols-3 gap-6">
      <div class="lg:col-span-2 bg-surface-container-high rounded-lg p-6 border border-outline-variant/10">
      <div class="flex items-center justify-between mb-6">
      <h3 class="headline-font font-bold text-on-surface flex items-center gap-2">
      <span class="material-symbols-outlined text-primary">timeline</span>
                    Schedule Activity Load
                  </h3>
      <span class="text-[10px] text-on-surface-variant uppercase tracking-widest">Last 24 Hours</span>
      </div>
      <div class="h-48 flex items-end gap-1 px-2">
      <!-- Simulated Histogram -->
      <div class="flex-1 bg-primary/20 h-1/2 rounded-t hover:bg-primary/40 transition-all"></div>
      <div class="flex-1 bg-primary/20 h-2/3 rounded-t hover:bg-primary/40 transition-all"></div>
      <div class="flex-1 bg-primary/20 h-3/4 rounded-t hover:bg-primary/40 transition-all"></div>
      <div class="flex-1 bg-primary/30 h-full rounded-t hover:bg-primary/50 transition-all"></div>
      <div class="flex-1 bg-primary/30 h-4/5 rounded-t hover:bg-primary/50 transition-all"></div>
      <div class="flex-1 bg-primary/20 h-1/3 rounded-t hover:bg-primary/40 transition-all"></div>
      <div class="flex-1 bg-primary/10 h-1/5 rounded-t hover:bg-primary/40 transition-all"></div>
      <div class="flex-1 bg-primary/10 h-1/6 rounded-t hover:bg-primary/40 transition-all"></div>
      <div class="flex-1 bg-primary/20 h-1/4 rounded-t hover:bg-primary/40 transition-all"></div>
      <div class="flex-1 bg-primary/30 h-2/3 rounded-t hover:bg-primary/50 transition-all"></div>
      <div class="flex-1 bg-primary/50 h-5/6 rounded-t hover:bg-primary/70 transition-all"></div>
      <div class="flex-1 bg-primary/40 h-3/4 rounded-t hover:bg-primary/60 transition-all"></div>
      </div>
      <div class="flex justify-between mt-4 px-2">
      <span class="text-[10px] text-slate-500 font-bold">00:00</span>
      <span class="text-[10px] text-slate-500 font-bold">06:00</span>
      <span class="text-[10px] text-slate-500 font-bold">12:00</span>
      <span class="text-[10px] text-slate-500 font-bold">18:00</span>
      <span class="text-[10px] text-slate-500 font-bold">23:59</span>
      </div>
      </div>
      <!-- Glassmorphism Inspector Card -->
      <div class="glass-inspector border border-[#2b4680]/30 rounded-lg p-6 flex flex-col justify-between">
      <div>
      <div class="flex items-center gap-2 mb-4">
      <span class="material-symbols-outlined text-tertiary">warning</span>
      <h3 class="text-sm font-bold text-on-surface uppercase tracking-tight">Active Overrides</h3>
      </div>
      <div class="space-y-4">
      <div class="p-3 bg-black/20 rounded border-l-2 border-tertiary">
      <p class="text-xs font-bold text-on-surface mb-1">Manual Occupancy: Floor 4</p>
      <p class="text-[10px] text-on-surface-variant">Initiated by: Operator_JohnD</p>
      <p class="text-[10px] text-tertiary font-bold mt-2 italic">Expires in 01h 12m</p>
      </div>
      <div class="p-3 bg-black/20 rounded border-l-2 border-primary">
      <p class="text-xs font-bold text-on-surface mb-1">Holiday Exception: Labor Day</p>
      <p class="text-[10px] text-on-surface-variant">Global System Exception</p>
      <p class="text-[10px] text-primary font-bold mt-2 uppercase">Scheduled: Sept 02</p>
      </div>
      </div>
      </div>
      <button class="mt-8 py-3 border border-outline-variant/30 text-xs font-bold text-on-surface hover:bg-white/5 transition-all rounded">
                  CONFIGURE CALENDAR EXCEPTIONS
                </button>
      </div>
      </div>
    </div>
    
    <!-- Right Property Inspector -->
    <aside class="hidden xl:flex flex-col w-80 bg-surface-container-highest border-l border-outline-variant/20">
      <div class="p-6">
      <h3 class="headline-font font-bold text-on-surface flex items-center gap-2 mb-6">
      <span class="material-symbols-outlined text-primary text-lg">info</span>
              Inspector
            </h3>
      <div class="space-y-6">
      <section>
      <p class="text-[10px] font-extrabold text-slate-500 uppercase tracking-widest mb-3">Selected Object</p>
      <div class="bg-surface-container-high rounded p-3 mb-2">
      <h4 class="text-sm font-bold text-primary">Office_Lighting_Sch</h4>
      <p class="text-[10px] text-slate-400 font-mono mt-1">BACNET_SCHEDULE:2104</p>
      </div>
      </section>
      <section>
      <p class="text-[10px] font-extrabold text-slate-500 uppercase tracking-widest mb-3">Priority Array</p>
      <div class="space-y-1">
      <div class="flex justify-between text-[11px] py-1 border-b border-outline-variant/10">
      <span class="text-slate-400">P1 (Manual Life Safety)</span>
      <span class="font-mono text-slate-500">NULL</span>
      </div>
      <div class="flex justify-between text-[11px] py-1 border-b border-outline-variant/10">
      <span class="text-slate-400">P8 (Manual Operator)</span>
      <span class="font-mono text-primary font-bold">1</span>
      </div>
      <div class="flex justify-between text-[11px] py-1 border-b border-outline-variant/10">
      <span class="text-slate-400">P16 (Scheduled)</span>
      <span class="font-mono text-slate-500">0</span>
      </div>
      </div>
      </section>
      <section>
      <p class="text-[10px] font-extrabold text-slate-500 uppercase tracking-widest mb-3">Object Reference</p>
      <div class="bg-surface-container-high rounded p-3">
      <div class="flex items-center gap-2 mb-2">
      <span class="material-symbols-outlined text-[14px] text-secondary">link</span>
      <span class="text-xs font-bold">Output List</span>
      </div>
      <ul class="text-[10px] text-slate-400 space-y-1 list-disc list-inside">
      <li>BO-12: Main Hall Lights</li>
      <li>BO-15: Open Office A</li>
      <li>BO-16: Open Office B</li>
      <li>AV-90: Dimming Level</li>
      </ul>
      </div>
      </section>
      <section>
      <img class="w-full rounded-lg border border-outline-variant/20 grayscale opacity-40 hover:grayscale-0 hover:opacity-100 transition-all duration-500" data-alt="A sophisticated technical architectural diagram showing a network of building controllers interconnected with glowing cyan fiber optic lines. The style is highly detailed and technical, resembling a blueprint but with modern dark-mode aesthetics, featuring floating data labels and semi-transparent layers that represent various building systems." src="https://lh3.googleusercontent.com/aida-public/AB6AXuASvF6ejd8xnYU9ueKnvWRVmc_lVdrPC4NHGwTZ4Yz5oxKADhcvGOJGXKbVaNVWMPy44v5akCQ2bUrJSRq2rkboH04PFEbnYm3iFcc2drW7Q0fWx1Xd1zc5x6UyRL3rTyejHZjfDyEil6EIhJ1lXolniKqab-p2D4IEVUXiBy6kgN4QD2KE-t4lBUdGOzk9d0HqxMR9oOHbRFoA1VR08Cmr6HBKFg6kQBMb6mfx0_I4AUTtzAU-TpdXplSvXu_8xjVGa3WONtos_Ig"/>
      <p class="text-[10px] text-center text-slate-600 mt-2 italic text-balance">Device Topology Location: B1-RACK-04</p>
      </section>
      </div>
      </div>
    </aside>
  </div>
</template>
