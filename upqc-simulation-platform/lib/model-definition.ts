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
  {
    id: 'microgrid',
    type: 'microgrid',
    position: { x: 50, y: 250 },
    data: {
      label: 'SOLAR PV ARRAY',
      type: 'microgrid',
      details: '7S×88P 415W Solar\nWind: 8.5 m/s\nBat SOC: 78%',
      parameters: {
        'Total PV': '255.6 kW',
      },
    },
  },
  {
    id: 'dc-link',
    type: 'electrical',
    position: { x: 250, y: 250 },
    data: {
      label: 'DC LINK = 700V',
      type: 'source',
      details: 'Shared DC Bus',
    },
  },
  {
    id: 'main-inverter',
    type: 'electrical',
    position: { x: 450, y: 250 },
    data: {
      label: 'MAIN INVERTER',
      type: 'inverter',
      details: 'Grid-Forming\nDC-AC Converter',
    },
  },
  {
    id: 'ac-microgrid',
    type: 'electrical',
    position: { x: 650, y: 250 },
    data: {
      label: 'AC MICROGRID BUS',
      type: 'source',
      details: 'Off-Grid AC Bus',
      parameters: {
        'Voltage': '415V RMS',
        'Freq': '60 Hz',
      },
    },
  },
  {
    id: 'critical-load',
    type: 'electrical',
    position: { x: 850, y: 150 },
    data: {
      label: 'CRITICAL LOAD',
      type: 'load',
      details: '15kW Linear Load',
    },
  },
  {
    id: 'diode-rectifier',
    type: 'electrical',
    position: { x: 850, y: 350 },
    data: {
      label: 'DIODE RECT',
      type: 'load',
      details: 'Non-Linear Load\nHarmonic Source\nTHD > 28%',
    },
  },
  {
    id: 'scope-block',
    type: 'scope',
    position: { x: 850, y: 50 },
    data: {
      label: 'SCOPE MON',
      type: 'scope',
      details: 'Oscilloscope Tap',
    },
  },
];

export const initialEdges: Edge[] = [
  {
    id: 'e-pv-dc',
    source: 'microgrid',
    target: 'dc-link',
    animated: true,
    style: { stroke: '#f59e0b', strokeWidth: 3 },
  },
  {
    id: 'e-dc-inv',
    source: 'dc-link',
    target: 'main-inverter',
    animated: true,
    style: { stroke: '#f59e0b', strokeWidth: 2.5 },
  },
  {
    id: 'e-inv-ac',
    source: 'main-inverter',
    target: 'ac-microgrid',
    animated: true,
    style: { stroke: '#10b981', strokeWidth: 2.5 },
  },
  {
    id: 'e-ac-cload',
    source: 'ac-microgrid',
    target: 'critical-load',
    animated: true,
    style: { stroke: '#00f0ff', strokeWidth: 2 },
  },
  {
    id: 'e-ac-diode',
    source: 'ac-microgrid',
    target: 'diode-rectifier',
    animated: true,
    style: { stroke: '#f43f5e', strokeWidth: 2 },
  },
  {
    id: 'e-ac-scope',
    source: 'ac-microgrid',
    target: 'scope-block',
    animated: true,
    style: { stroke: '#00f0ff', strokeWidth: 1.5, strokeDasharray: '3' },
  },
];
