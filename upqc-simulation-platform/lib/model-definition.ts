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
    id: 'grid-source',
    type: 'electrical',
    position: { x: 50, y: 150 },
    data: {
      label: 'MAIN GRID',
      type: 'source',
      details: '415V RMS L-L\n50 Hz, 3-Phase',
      parameters: { 'Voltage': '415V', 'Frequency': '50Hz' },
    },
  },
  {
    id: 'series-trans',
    type: 'electrical',
    position: { x: 250, y: 150 },
    data: {
      label: 'SERIES TRANSFORMER',
      type: 'transformer',
      details: 'Voltage Injection\nRatio: 1:1',
    },
  },
  {
    id: 'series-inv',
    type: 'electrical',
    position: { x: 250, y: 350 },
    data: {
      label: 'SERIES INVERTER',
      type: 'inverter',
      details: 'Voltage Sag/Swell\nCompensation',
    },
  },
  {
    id: 'ac-bus',
    type: 'electrical',
    position: { x: 650, y: 150 },
    data: {
      label: 'LOAD BUS (PCC)',
      type: 'source',
      details: 'Point of Common\nCoupling',
    },
  },
  {
    id: 'shunt-inv',
    type: 'electrical',
    position: { x: 650, y: 350 },
    data: {
      label: 'SHUNT INVERTER',
      type: 'inverter',
      details: 'Harmonic & Reactive\nPower Compensation',
    },
  },
  {
    id: 'dc-link',
    type: 'electrical',
    position: { x: 450, y: 500 },
    data: {
      label: 'DC LINK = 700V',
      type: 'source',
      details: 'Shared DC Bus\nCapacitor: 3mF',
    },
  },
  {
    id: 'microgrid',
    type: 'microgrid',
    position: { x: 350, y: 650 },
    data: {
      label: 'SOLAR PV ARRAY',
      type: 'microgrid',
      details: '7S × 88P × 415Wp\n= 255.6 kW (STC)',
      parameters: { solarStringsParallel: 88, solarModulesSeries: 7, solarPanelWatts: 415, solarVmpp: 34.1 },
    },
  },
  {
    id: 'battery-storage',
    type: 'electrical',
    position: { x: 600, y: 650 },
    data: {
      label: 'BATTERY BESS',
      type: 'source',
      details: '100kWh BESS\nParallel DC Bus',
      parameters: { batterySOC: 80, batteryCapacityKwh: 100 },
      active: false,
    },
  },
  {
    id: 'wind-turbine',
    type: 'microgrid',
    position: { x: 150, y: 650 },
    data: {
      label: 'WIND TURBINE',
      type: 'microgrid',
      details: '50kW Turbine\nParallel AC Bus',
      parameters: { windSpeed: 8.0, windCutIn: 3.0, windCutOut: 25.0, windNominalPower: 50.0 },
      active: false,
    },
  },
  {
    id: 'critical-load',
    type: 'electrical',
    position: { x: 950, y: 50 },
    data: {
      label: 'LINEAR LOAD',
      type: 'load',
      details: '15kW, 10kVAR',
    },
  },
  {
    id: 'diode-rectifier',
    type: 'electrical',
    position: { x: 950, y: 250 },
    data: {
      label: 'NON-LINEAR LOAD',
      type: 'load',
      details: 'Diode Rectifier\nHarmonic Source\nTHD > 28%',
    },
  },
  {
    id: 'scope-block',
    type: 'scope',
    position: { x: 950, y: -50 },
    data: {
      label: 'SCOPE MON',
      type: 'scope',
      details: 'V/I Waveforms\nOscilloscope Tap',
    },
  },
];

export const initialEdges: Edge[] = [
  {
    id: 'e-grid-trans',
    source: 'grid-source',
    target: 'series-trans',
    sourceHandle: 'r-s',
    targetHandle: 'l-t',
    animated: true,
    style: { stroke: '#0ea5e9', strokeWidth: 2.5 },
  },
  {
    id: 'e-trans-bus',
    source: 'series-trans',
    target: 'ac-bus',
    sourceHandle: 'r-s',
    targetHandle: 'l-t',
    animated: true,
    style: { stroke: '#0ea5e9', strokeWidth: 2.5 },
  },
  {
    id: 'e-series-inv-trans',
    source: 'series-inv',
    target: 'series-trans',
    sourceHandle: 't-s',
    targetHandle: 'b-t',
    animated: true,
    style: { stroke: '#10b981', strokeWidth: 2 },
  },
  {
    id: 'e-shunt-inv-bus',
    source: 'shunt-inv',
    target: 'ac-bus',
    sourceHandle: 't-s',
    targetHandle: 'b-t',
    animated: true,
    style: { stroke: '#8b5cf6', strokeWidth: 2 },
  },
  {
    id: 'e-dc-series',
    source: 'dc-link',
    target: 'series-inv',
    sourceHandle: 'l-s',
    targetHandle: 'r-t',
    animated: true,
    style: { stroke: '#f59e0b', strokeWidth: 2 },
  },
  {
    id: 'e-dc-shunt',
    source: 'dc-link',
    target: 'shunt-inv',
    sourceHandle: 'r-s',
    targetHandle: 'l-t',
    animated: true,
    style: { stroke: '#f59e0b', strokeWidth: 2 },
  },
  {
    id: 'e-pv-dc',
    source: 'microgrid',
    target: 'dc-link',
    sourceHandle: 't-s',
    targetHandle: 'b-t',
    animated: true,
    style: { stroke: '#f59e0b', strokeWidth: 3 },
  },
  {
    id: 'e-bus-load',
    source: 'ac-bus',
    target: 'critical-load',
    sourceHandle: 'r-s',
    targetHandle: 'l-t',
    animated: true,
    style: { stroke: '#0284c7', strokeWidth: 2 },
  },
  {
    id: 'e-bus-nonlinear',
    source: 'ac-bus',
    target: 'diode-rectifier',
    sourceHandle: 'r-s',
    targetHandle: 'l-t',
    animated: true,
    style: { stroke: '#e11d48', strokeWidth: 2 },
  },
  {
    id: 'e-bus-scope',
    source: 'ac-bus',
    target: 'scope-block',
    sourceHandle: 't-s',
    targetHandle: 'l-t',
    animated: true,
    style: { stroke: '#64748b', strokeWidth: 1.5, strokeDasharray: '4' },
  },
  // Battery BESS connects in PARALLEL to DC Link (shared DC bus — both absorb/inject DC power)
  {
    id: 'e-battery-dc',
    source: 'battery-storage',
    target: 'dc-link',
    sourceHandle: 't-s',
    targetHandle: 'b-t',
    animated: true,
    style: { stroke: '#10b981', strokeWidth: 2.5 },
    label: '∥ DC Bus',
    labelStyle: { fontSize: 9, fill: '#10b981', fontWeight: 'bold' },
  },
  // Wind turbine connects in PARALLEL to AC Bus (it is a voltage-following inverter at PCC)
  {
    id: 'e-wind-ac',
    source: 'wind-turbine',
    target: 'ac-bus',
    sourceHandle: 'r-s',
    targetHandle: 'l-t',
    animated: true,
    style: { stroke: '#06b6d4', strokeWidth: 2.5 },
    label: '∥ AC Bus',
    labelStyle: { fontSize: 9, fill: '#06b6d4', fontWeight: 'bold' },
  },
];
