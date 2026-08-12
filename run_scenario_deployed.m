function result = run_scenario_deployed(profileName, tempVal)
% RUN_SCENARIO_DEPLOYED  Deployment-ready version for Simulink Compiler / Python packaging.
% Runs PV_Array_Model in Rapid Accelerator mode with the given irradiance
% profile and temperature, returns a struct of numeric arrays (Python-friendly).
%
%   result = run_scenario_deployed('DeepDip_Recover', 35)
%
% result fields: Time, Vrms (Nx3), Irms (Nx3), Irradiance, Pac, Q,
%                THDv (Nx3), THDi (Nx3), Vdc, Ipv, Vpv, Temp, Pdc

mdl      = 'PV_Array_Model';
f_grid   = 60;
Ts_cycle = 1/f_grid;
t_transient = 0.3;
varNames = {'Vrms','Irms','Irradiance','Pac','Q','THDv','THDi','Vdc','Ipv','Vpv','Temp'};

tCommon = [0 0.2 0.4 0.6 0.8 1.0];
profileTable = containers.Map();
profileTable('DeepDip_Recover') = [1000 1000 200 200 1000 1000];
profileTable('RampDown')        = [1000 850  700 550  400  250];
profileTable('RampUp')          = [200  350  500 650  800 1000];
profileTable('ShallowCloud')    = [950  850  700 700  800  900];
profileTable('DoubleDip')       = [1000 300  900 300  900 1000];

if ~isKey(profileTable, profileName)
    error('Unknown profile "%s". Valid: %s', profileName, strjoin(keys(profileTable), ', '));
end
eVals = profileTable(profileName);

if tempVal < -20 || tempVal > 85
    error('Temperature %g out of realistic range (-20 to 85 C).', tempVal);
end

% Deployment-required pattern: use setVariable (NOT setBlockParameter --
% these are incompatible with configureForDeployment). This requires the
% Irradiance block's t/e fields and the Temperature block's Value field
% to reference workspace variable names (IrrT, IrrE, TempC) rather than
% literal values -- see model setup steps in project notes.

in = Simulink.SimulationInput(mdl);
in = in.setVariable('IrrT', tCommon);
in = in.setVariable('IrrE', eVals);
in = in.setVariable('TempC', tempVal);
in = simulink.compiler.configureForDeployment(in);   % must come AFTER setVariable calls

out = sim(in);

% ---- Extract signals directly from Simulink.SimulationOutput (no base workspace needed) ----
nSamples = zeros(size(varNames));
for i = 1:numel(varNames)
    ts = out.get(varNames{i});
    nSamples(i) = numel(ts.Time);
end

multiIdx = find(nSamples >= 2);
tStart = max(arrayfun(@(i) min(out.get(varNames{i}).Time), multiIdx));
tEnd   = min(arrayfun(@(i) max(out.get(varNames{i}).Time), multiIdx));
tGrid  = (tStart + t_transient : Ts_cycle : tEnd)';

result = struct();
result.Time = tGrid;

for i = 1:numel(varNames)
    ts = out.get(varNames{i});
    t = ts.Time; y = ts.Data;
    y = reshape(y, size(y,1), []);

    if size(y,1) == 1
        vals = repmat(y, numel(tGrid), 1);
    else
        [t, uidx] = unique(t, 'last');
        y = y(uidx, :);
        vals = interp1(t, y, tGrid, 'previous', 'extrap');
    end
    result.(varNames{i}) = vals;
end

result.Pdc = result.Vpv .* result.Ipv;

end
