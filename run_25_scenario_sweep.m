%% run_25_scenario_sweep.m
% Automates 25 simulation runs of PV_Array_Model.slx:
%   5 dynamic irradiance profiles x 5 temperature levels
% Each run's per-cycle data is extracted (same logic as collect_lstm_data.m),
% tagged with scenario metadata, and all 25 runs are combined into one CSV.

clear; clc;

%% ---- USER SETTINGS ----
mdl          = 'PV_Array_Model';
f_grid       = 60;                 % grid frequency (Hz) -- matches Fnom/RMS/THD blocks
Ts_cycle     = 1/f_grid;
t_transient  = 0.3;                % seconds discarded at start of each run (settling time)
outFile      = 'lstm_training_data_25scenarios.csv';

varNames = {'Vrms','Irms','Irradiance','Pac','Q','THDv','THDi','Vdc','Ipv','Vpv','Temp'};

%% ---- IRRADIANCE PROFILES (Stair Generator t/e pairs) ----
% All profiles use the SAME number of breakpoints (6) so that switching
% between profiles never leaves 't' and 'e' at mismatched lengths mid-update
% (Simulink validates t/e length match immediately on each set_param call).
tCommon = [0 0.2 0.4 0.6 0.8 1.0];

profiles = struct('name', {}, 't', {}, 'e', {});
profiles(1) = struct('name','DeepDip_Recover', 't', tCommon, 'e', [1000 1000 200 200 1000 1000]);
profiles(2) = struct('name','RampDown',        't', tCommon, 'e', [1000 850  700 550  400  250]);
profiles(3) = struct('name','RampUp',          't', tCommon, 'e', [200  350  500 650  800 1000]);
profiles(4) = struct('name','ShallowCloud',    't', tCommon, 'e', [950  850  700 700  800  900]);
profiles(5) = struct('name','DoubleDip',       't', tCommon, 'e', [1000 300  900 300  900 1000]);

%% ---- TEMPERATURE LEVELS ----
tempLevels = [15 25 35 45 55];   % deg C

%% ---- LOCATE BLOCKS ----
load_system(mdl);
irrBlock  = find_system(mdl, 'SearchDepth', 1, 'RegExp', 'on', 'Name', '^Irradiance');
tempBlock = find_system(mdl, 'SearchDepth', 1, 'RegExp', 'on', 'Name', '^Temperature');
if isempty(irrBlock) || isempty(tempBlock)
    error('Could not locate Irradiance or Temperature blocks at the top level of %s. Check block names.', mdl);
end
irrBlock  = irrBlock{1};
tempBlock = tempBlock{1};
fprintf('Irradiance block: %s\n', irrBlock);
fprintf('Temperature block: %s\n', tempBlock);

%% ---- SWEEP ----
if isfile('sweep_progress.mat')
    resumeAns = input('Found sweep_progress.mat from a previous run. Resume from there? (y/n): ', 's');
else
    resumeAns = 'n';
end

if strcmpi(resumeAns, 'y')
    load('sweep_progress.mat', 'allRuns', 'scenarioLog', 'scenarioID');
    startID = scenarioID + 1;
    fprintf('Resuming from scenario %d.\n', startID);
else
    allRuns = {};
    scenarioLog = table();
    startID = 1;
end

scenarioID = 0;
for p = 1:numel(profiles)
    for k = 1:numel(tempLevels)
        scenarioID = scenarioID + 1;
        if scenarioID < startID
            continue;   % already completed in a previous run, skip
        end
        profName = profiles(p).name;
        tVal = tempLevels(k);

        fprintf('\n=== Scenario %d/25: profile=%s, Temp=%dC ===\n', scenarioID, profName, tVal);

        try
            % Set irradiance stair profile
            set_param(irrBlock, 't', mat2str(profiles(p).t));
            set_param(irrBlock, 'e', mat2str(profiles(p).e));

            % Set temperature constant
            set_param(tempBlock, 'Value', num2str(tVal));

            simOut = sim(mdl);  %#ok<NASGU>  % To Workspace (Timeseries) blocks populate base workspace

            % ---- Extract per-cycle data for this run ----
            runTable = extract_run_data(varNames, t_transient, Ts_cycle);
        catch ME
            warning('Scenario %d (%s, %dC) FAILED: %s. Skipping.', scenarioID, profName, tVal, ME.message);
            continue;
        end

        runTable.ScenarioID   = repmat(scenarioID, height(runTable), 1);
        runTable.ProfileName  = repmat(string(profName), height(runTable), 1);
        runTable.TempSetpoint = repmat(tVal, height(runTable), 1);

        allRuns{end+1} = runTable; %#ok<SAGROW>

        scenarioLog = [scenarioLog; table(scenarioID, string(profName), tVal, height(runTable), ...
            'VariableNames', {'ScenarioID','ProfileName','TempSetpoint','Rows'})]; %#ok<AGROW>

        % ---- INCREMENTAL SAVE: write progress after every scenario ----
        % so a crash later in the sweep never loses completed work.
        master_partial = vertcat(allRuns{:});
        writetable(master_partial, outFile);
        save('sweep_progress.mat', 'allRuns', 'scenarioLog', 'scenarioID');
        fprintf('  -> saved progress (%d rows so far) to %s\n', height(master_partial), outFile);
    end
end

%% ---- COMBINE AND EXPORT ----
if isempty(allRuns)
    error('No scenarios completed successfully -- nothing to export.');
end

master = vertcat(allRuns{:});
writetable(master, outFile);

fprintf('\n===== DONE =====\n');
fprintf('Combined %d scenarios, %d total rows -> %s\n', height(scenarioLog), height(master), outFile);
disp(scenarioLog);
