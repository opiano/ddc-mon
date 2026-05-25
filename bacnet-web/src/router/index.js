import { createRouter, createWebHistory } from 'vue-router'
import AppLayout from '../layouts/AppLayout.vue'
import SystemOverview from '../views/SystemOverview.vue'
import AnalogInputs from '../views/AnalogInputs.vue'
import AnalogOutputs from '../views/AnalogOutputs.vue'
import AnalogValues from '../views/AnalogValues.vue'
import BinaryInputs from '../views/BinaryInputs.vue'
import BinaryOutputs from '../views/BinaryOutputs.vue'
import BinaryValues from '../views/BinaryValues.vue'
import MultiStateValues from '../views/MultiStateValues.vue'
import Schedules from '../views/Schedules.vue'
import TrendLogs from '../views/TrendLogs.vue'
import Calendars from '../views/Calendars.vue'
import Fbd from '../views/Fbd.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      component: AppLayout,
      children: [
        {
          path: '',
          name: 'system-overview',
          component: SystemOverview
        },
        {
          path: 'analog-inputs',
          name: 'analog-inputs',
          component: AnalogInputs
        },
        {
          path: 'analog-outputs',
          name: 'analog-outputs',
          component: AnalogOutputs
        },
        {
          path: 'analog-values',
          name: 'analog-values',
          component: AnalogValues
        },
        {
          path: 'binary-inputs',
          name: 'binary-inputs',
          component: BinaryInputs
        },
        {
          path: 'binary-outputs',
          name: 'binary-outputs',
          component: BinaryOutputs
        },
        {
          path: 'binary-values',
          name: 'binary-values',
          component: BinaryValues
        },
        {
          path: 'multi-state-values',
          name: 'multi-state-values',
          component: MultiStateValues
        },
        {
          path: 'schedules',
          name: 'schedules',
          component: Schedules
        },
        {
          path: 'trend-logs',
          name: 'trend-logs',
          component: TrendLogs
        },
        {
          path: 'calendars',
          name: 'calendars',
          component: Calendars
        },
        {
          path: 'fbd',
          name: 'fbd',
          component: Fbd
        }
      ]
    }
  ]
})

export default router
