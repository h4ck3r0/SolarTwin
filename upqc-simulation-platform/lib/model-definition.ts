import { Node, Edge } from 'reactflow';

export interface ElectricalNodeData {
  label: string;
  type: 'source' | 'measurement' | 'transformer' | 'inverter' | 'control' | 'microgrid' | 'load' | 'scope';
  status?: string;
  details?: string;
  parameters?: Record<string, string | number>;
  active?: boolean;
}

export const initialNodes: Node<ElectricalNodeData>[] = [
  // 1. Grid Source
  {
    id: 'grid-source',
    type: 'electrical',
    position: { x: 50, y: 150 },
    data: {
      label: '3Φ GRID SOURCE',
      type: 'source',
      details: '415V LL / 50Hz',
      parameters: {
        'Rs': '0.1 Ω',
        'Ls': '0.001 H',
      },
    },
  },

  // 2. Series Transformer
  {
    id: 'series-trans',
    type: 'electrical',
    position: { x: 230, y: 150 },
    data: {
      label: 'SERIES XFM',
      type: 'transformer',
      details: '1:1 Coupling',
    },
  },

  // 3. Series APF Inverter (Green)
  {
    id: 'series-inv',
    type: 'electrical',
    position: { x: 400, y: 140 },
    data: {
      label: 'SERIES APF',
      type: 'inverter',
      details: 'PI Controller\nIGBT Inverter',
      parameters: {
        'Kp': 1.5,
        'Ki': 120,
      },
    },
  },

  // 4. Shunt APF Inverter (Purple)
  {
    id: 'shunt-inv',
    type: 'electrical',
    position: { x: 580, y: 220 },
    data: {
      label: 'SHUNT APF',
      type: 'control',
      details: 'PI Controller\nIGBT Inverter',
      parameters: {
        'Kp': 1.0,
        'Ki': 85,
      },
    },
  },

  // 5. DC Link Capacitor (Orange Bar)
  {
    id: 'dc-link',
    type: 'electrical',
    position: { x: 490, y: 350 },
    data: {
      label: 'DC LINK = 700V',
      type: 'source',
      details: 'Shared DC Bus',
    },
  },

  // 6. Microgrid Sources (Yellow)
  {
    id: 'microgrid',
    type: 'microgrid',
    position: { x: 480, y: 470 },
    data: {
      label: 'MICROGRID',
      type: 'microgrid',
      details: '5×305W Solar\nWind: 8.5 m/s\nBat SOC: 78%',
      parameters: {
        'Total PV': '1.525 kW',
      },
    },
  },

  // 7. Critical Load (Green)
  {
    id: 'critical-load',
    type: 'electrical',
    position: { x: 740, y: 130 },
    data: {
      label: 'CRITICAL LOAD',
      type: 'load',
      details: '15kW Linear Load\nVref=230V',
    },
  },

  // 8. Diode Rectifier Non-Linear Load (Red)
  {
    id: 'diode-rectifier',
    type: 'electrical',
    position: { x: 890, y: 130 },
    data: {
      label: 'DIODE RECT',
      type: 'load',
      details: 'Non-Linear Load\nHarmonic Source\nTHD > 28%',
    },
  },

  // 9. Scope Monitor (Cyan)
  {
    id: 'scope-block',
    type: 'scope',
    position: { x: 750, y: 320 },
    data: {
      label: 'SCOPE MON',
      type: 'scope',
      details: 'Oscilloscope Tap',
    },
  },
];

export const initialEdges: Edge[] = [
  // Grid to Series Transformer
  {
    id: 'e-grid-xfm',
    source: 'grid-source',
    target: 'series-trans',
    animated: true,
    style: { stroke: '#00f0ff', strokeWidth: 2 },
  },
  // Series Transformer to Series APF
  {
    id: 'e-xfm-sinv',
    source: 'series-trans',
    target: 'series-inv',
    animated: true,
    style: { stroke: '#00f0ff', strokeWidth: 2 },
  },
  // Series APF to Shunt APF
  {
    id: 'e-sinv-shinv',
    source: 'series-inv',
    target: 'shunt-inv',
    animated: true,
    style: { stroke: '#00f0ff', strokeWidth: 2 },
  },
  // Series APF down to DC Link
  {
    id: 'e-sinv-dc',
    source: 'series-inv',
    target: 'dc-link',
    animated: true,
    style: { stroke: '#f59e0b', strokeWidth: 2.5 },
  },
  // Shunt APF down to DC Link
  {
    id: 'e-shinv-dc',
    source: 'shunt-inv',
    target: 'dc-link',
    animated: true,
    style: { stroke: '#f59e0b', strokeWidth: 2.5 },
  },
  // Microgrid up to DC Link
  {
    id: 'e-[#00f0ff]',
    source: 'microgrid',
    target: 'dc-link',
    animated: true,
    style: { stroke: '#f59e0b', strokeWidth: 3 },
  },
  // Shunt APF to Critical Load
  {
    id: 'e-shinv-cload',
    source: 'shunt-inv',
    target: 'critical-load',
    animated: true,
    style: { stroke: '#10b981', strokeWidth: 2, strokeDasharray: '4' },
  },
  // Critical Load to Diode Rectifier
  {
    id: 'e-cload-diode',
    source: 'critical-load',
    target: 'diode-rectifier',
    animated: true,
    style: { stroke: '#f43f5e', strokeWidth: 2 },
  },
  // Scope monitor tap
  {
    id: 'e-cload-scope',
    source: 'critical-load',
    target: 'scope-block',
    animated: true,
    style: { stroke: '#00f0ff', strokeWidth: 1.5, strokeDasharray: '3' },
  },
];
