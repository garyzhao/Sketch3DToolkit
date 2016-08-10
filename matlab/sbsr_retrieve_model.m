%% Setup dataset variables.
setupvars;
[sketchclass, ~] = read_classification_file(DATASET_CONFIG_SKETCH_FILE);
[modelclass, ~] = read_classification_file(DATASET_CONFIG_MODEL_FILE);
modelclass(:, 1) = strcat('m', modelclass(:, 1));

if ~exist(RANK_DIR, 'dir')
    mkdir(RANK_DIR);
end

%% Create the feature index for model views.
mexHashIndexer('init');
viewfiles = dir(fullfile(DATASET_VIEW_DIR, '*.png'));
target_num = size(viewfiles, 1);
for i = 1:target_num 
    tic;
    target_path = viewfiles(i).name;
    [~, model_name, ~] = fileparts(target_path);
    mexHashIndexer('insert', fullfile(DATASET_VIEW_DIR, target_path), model_name);
    dt = toc;
    fprintf('Done #%d in %f seconds.\n', i, dt);
end
mexHashIndexer('save', INDEXER_DATA_FILE);

%% Retrieve 3D models for each sketch query.
% mexHashIndexer('load', INDEXER_DATA_FILE);
query_num = size(sketchclass, 1);
t = 0;
for i = 1:query_num
    tic;
    query_name = sketchclass{i, 1};
    query_class = sketchclass{i, 2};
    target_query_path = fullfile(DATASET_SKETCH_DIR, query_class, 'test', [query_name, '.png']);
    mexHashIndexer('retrieve_model', target_query_path, query_name, fullfile(RANK_DIR, query_name));
    dt = toc;
    t = t + dt;
    fprintf('Done #%d in %f seconds.\n', i, dt);
end
fprintf('Average %f s/m.\n', t / query_num);
