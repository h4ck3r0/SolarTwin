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
  // Electrical Network (Main Horizontal Path)
  {
    id: 'grid-source',
    type: 'electrical',
    position: { x: 40, y: 150 },
    data: {
      label: 'Grid Source',
      type: 'source',
      details: '3-Phase AC Source, 415V LL, 50Hz',
      parameters: {
        'Voltage (V)': 415,
        'Frequency (Hz)': 50,
        'R_s (Ω)': 0.1,
        'L_s (H)': 0.001,
      },
    },
  },
  {
    id: 'grid-meas',
    type: 'electrical',
    position: { x: 230, y: 150 },
    data: {
      label: 'Grid Measurement',
      type: 'measurement',
      details: 'V_grid, I_grid 3-Phase Scopes',
    },
  },
  {
    id: 'series-trans',
    type: 'electrical',
    position: { x: 420, y: 140 },
    data: {
      label: 'Series Transformer',
      type: 'transformer',
      details: 'Series Injection Coupling Transformer',
      parameters: {
        'Turns Ratio': '1:1',
        'Power Rating (kVA)': 15,
      },
    },
  },
  {
    id: 'load-meas',
    type: 'electrical',
    position: { x: 620, y: 150 },
    data: {
      label: 'Load Measurement',
      type: 'measurement',
      details: 'V_load, I_load 3-Phase PCC Scopes',
    },
  },
  {
    id: 'diode-rectifier',
    type: 'electrical',
    position: { x: 820, y: 240 },
    data: {
      label: 'Diode Rectifier',
      type: 'load',
      details: 'Non-linear Harmonic Load',
      parameters: {
        'Rectifier Type': '6-Pulse Diode',
        'R_load (Ω)': 15,
        'C_filter (µF)': 470,
      },
    },
  },
  {
    id: 'linear-load',
    type: 'electrical',
    position: { x: 820, y: 80 },
    data: {
      label: 'Critical Load',
      type: 'load',
      details: 'Sensitive Linear Load',
      parameters: {
        'Active Power (kW)': 8,
        'Reactive Power (kVAR)': 2,
      },
    },
  },

  // Series APF Branch (Bottom Left)
  {
    id: 'series-ctrl',
    type: 'control',
    position: { x: 230, y: 310 },
    data: {
      label: 'Series APF Control',
      type: 'control',
      details: 'PI Voltage Sag Controller',
      parameters: {
        'K_p': 1.5,
        'K_i': 120,
        'Ref Voltage (V)': 415,
      },
    },
  },
  {
    id: 'series-inv',
    type: 'electrical',
    position: { x: 420, y: 300 },
    data: {
      label: 'IGBT Inverter Series',
      type: 'inverter',
      details: 'Series Voltage Injector (APF)',
      parameters: {
        'IGBT Count': 6,
        'Snubber R (Ω)': 500,
      },
    },
  },

  // Shunt APF Branch (Bottom Right)
  {
    id: 'shunt-ctrl',
    type: 'control',
    position: { x: 820, y: 440 },
    data: {
      label: 'Shunt APF Control',
      type: 'control',
      details: 'PI Current Harmonic Mitigator',
      parameters: {
        'K_p': 1.0,
        'K_i': 85,
      },
    },
  },
  {
    id: 'shunt-inv',
    type: 'electrical',
    position: { x: 620, y: 300 },
    data: {
      label: 'IGBT Inverter Shunt',
      type: 'inverter',
      details: 'Shunt Current Injector (APF)',
      parameters: {
        'IGBT Count': 6,
        'Filter L (H)': 0.003,
      },
    },
  },

  // DC Link & Microgrid (Center Bottom)
  {
    id: 'dc-link',
    type: 'electrical',
    position: { x: 520, y: 440 },
    data: {
      label: 'DC Link Capacitor',
      type: 'source',
      details: 'Shared DC Capacitor Link',
      parameters: {
        'Target DC (V)': 700,
        'Capacitance (µF)': 2200,
      },
    },
  },
  {
    id: 'microgrid',
    type: 'microgrid',
    position: { x: 520, y: 580 },
    data: {
      label: 'Microgrid Sources',
      type: 'microgrid',
      details: 'Solar PV + Wind + Battery System',
      parameters: {
        'Solar Irradiance (W/m²)': 1000,
        'Wind Speed (m/s)': 12,
        'Battery SOC (%)': 80,
      },
    },
  },
  {
    id: 'dc-volt-ctrl',
    type: 'control',
    position: { x: 670, y: 560 },
    data: {
      label: 'Voltage Control',
      type: 'control',
      details: 'DC Link Voltage Regulator',
      parameters: {
        'K_p': 0.8,
        'K_i': 15,
      },
    },
  },

  // Scopes (Visual Monitoring Nodes)
  {
    id: 'scope-block',
    type: 'scope',
    position: { x: 1040, y: 150 },
    data: {
      label: 'Scopes Monitor',
      type: 'scope',
      details: 'Multi-trace Simulation Oscilloscope',
    },
  },
];

export const initialEdges: Edge[] = [
  // Main Power Flow Path (3-Phase)
  {
    id: 'e-grid-meas',
    source: 'grid-source',
    target: 'grid-meas',
    animated: false,
    label: '3Φ AC',
    style: { stroke: '#0f172a', strokeWidth: 2 },
  },
  {
    id: 'e-meas-trans',
    source: 'grid-meas',
    target: 'series-trans',
    animated: false,
    label: '3Φ AC',
    style: { stroke: '#0f172a', strokeWidth: 2 },
  },
  {
    id: 'e-trans-meas',
    source: 'series-trans',
    target: 'load-meas',
    animated: false,
    label: '3Φ AC',
    style: { stroke: '#0f172a', strokeWidth: 2 },
  },
  {
    id: 'e-meas-linear',
    source: 'load-meas',
    target: 'linear-load',
    animated: false,
    style: { stroke: '#0f172a', strokeWidth: 2 },
  },
  {
    id: 'e-meas-rectifier',
    source: 'load-meas',
    target: 'diode-rectifier',
    animated: false,
    style: { stroke: '#0f172a', strokeWidth: 2 },
  },

  // Series APF Path
  {
    id: 'e-inv-trans',
    source: 'series-inv',
    target: 'series-trans',
    animated: false,
    label: 'V_inj',
    style: { stroke: '#475569', strokeWidth: 1.5, strokeDasharray: '4' },
  },
  {
    id: 'e-meas-sctrl',
    source: 'grid-meas',
    target: 'series-ctrl',
    animated: false,
    style: { stroke: '#2563eb', strokeWidth: 1, strokeDasharray: '3' },
  },
  {
    id: 'e-sctrl-inv',
    source: 'series-ctrl',
    target: 'series-inv',
    animated: false,
    label: 'PWM',
    style: { stroke: '#2563eb', strokeWidth: 1 },
  },

  // Shunt APF Path
  {
    id: 'e-meas-shinv',
    source: 'load-meas',
    target: 'shunt-inv',
    animated: false,
    label: 'I_inj',
    style: { stroke: '#475569', strokeWidth: 1.5, strokeDasharray: '4' },
  },
  {
    id: 'e-meas-shctrl',
    source: 'load-meas',
    target: 'shunt-ctrl',
    animated: false,
    style: { stroke: '#2563eb', strokeWidth: 1, strokeDasharray: '3' },
  },
  {
    id: 'e-shctrl-inv',
    source: 'shunt-ctrl',
    target: 'shunt-inv',
    animated: false,
    label: 'PWM',
    style: { stroke: '#2563eb', strokeWidth: 1 },
  },

  // DC Link Connections
  {
    id: 'e-sinv-dc',
    source: 'series-inv',
    target: 'dc-link',
    animated: false,
    label: 'DC Link',
    style: { stroke: '#ea580c', strokeWidth: 1.5 },
  },
  {
    id: 'e-shinv-dc',
    source: 'shunt-inv',
    target: 'dc-link',
    animated: false,
    label: 'DC Link',
    style: { stroke: '#ea580c', strokeWidth: 1.5 },
  },
  {
    id: 'e-mg-dc',
    source: 'microgrid',
    target: 'dc-link',
    animated: false,
    label: 'Gen DC',
    style: { stroke: '#ea580c', strokeWidth: 1.5 },
  },

  // DC Voltage Control Path
  {
    id: 'e-dc-dcctrl',
    source: 'dc-link',
    target: 'dc-volt-ctrl',
    animated: false,
    style: { stroke: '#2563eb', strokeWidth: 1, strokeDasharray: '3' },
  },
  {
    id: 'e-dcctrl-shctrl',
    source: 'dc-volt-ctrl',
    target: 'shunt-ctrl',
    animated: false,
    style: { stroke: '#2563eb', strokeWidth: 1 },
  },

  // Scopes Connections
  {
    id: 'e-gmeas-scope',
    source: 'grid-meas',
    target: 'scope-block',
    animated: false,
    style: { stroke: '#16a34a', strokeWidth: 1, strokeDasharray: '5' },
  },
  {
    id: 'e-lmeas-scope',
    source: 'load-meas',
    target: 'scope-block',
    animated: false,
    style: { stroke: '#16a34a', strokeWidth: 1, strokeDasharray: '5' },
  },
  {
    id: 'e-dc-scope',
    source: 'dc-link',
    target: 'scope-block',
    animated: false,
    style: { stroke: '#16a34a', strokeWidth: 1, strokeDasharray: '5' },
  },
];
