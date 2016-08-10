%% Setup dataset variables.
setupvars;
[sketchclass, Ns] = read_classification_file(DATASET_CONFIG_SKETCH_FILE);
[modelclass, N] = read_classification_file(DATASET_CONFIG_MODEL_FILE);

if ~exist(DATASET_VIEW_DIR, 'dir')
    mkdir(DATASET_VIEW_DIR);
end

%% Select candidate views for each 3D model.
mexSelectMeshViews('init_context');
for i = 1:size(modelclass, 1)
    target_model = ['m' modelclass{i, 1}];
    target_class = modelclass{i, 2};
    
    idx = strcmp(sketchclass(:, 2), target_class);
    target_sketch = sketchclass(idx, 1);
    target_sketch_path = strcat(DATASET_SKETCH_DIR, ['/' target_class '/test/'], target_sketch, '.png');
    target_model_path = fullfile(DATASET_MODEL_DIR, [target_model '.off']);
    
    tic;
    mexSelectMeshViews('select_view', target_model_path, target_model, target_sketch_path, DATASET_VIEW_DIR);
    dt = toc;
    fprintf('Done #%d in %f seconds.\n', i, dt);
    drawnow('update');
end
mexSelectMeshViews('clear_context');
fprintf('Done.\n');
