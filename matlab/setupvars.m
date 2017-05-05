% Setup essential variables for SHREC'13 and SHREC'14
clear all; clc;

DATASET_DIR = 'SHREC13';
DATASET_CONFIG_SKETCH_FILE  = fullfile(DATASET_DIR, [DATASET_DIR, '_SBR_Sketch_Test.cla']);
DATASET_CONFIG_MODEL_FILE   = fullfile(DATASET_DIR, [DATASET_DIR, '_SBR_Model.cla']);

DATASET_VIEW_DIR                = fullfile(DATASET_DIR, 'Views');
DATASET_MODEL_DIR               = fullfile(DATASET_DIR, 'Models');
DATASET_SKETCH_DIR              = fullfile(DATASET_DIR, 'Sketches');
DATASET_PERFORMANCE_DATA_DIR    = fullfile(DATASET_DIR, 'PerformanceData');

RANK_DIR            = fullfile(DATASET_DIR, 'RetrievalLists');
RANK_STATS_FILE     = fullfile(RANK_DIR, 'Stats_test.txt');
RANK_PRC_FILE       = fullfile(RANK_DIR, 'PR_test.txt');
INDEXER_DATA_FILE   = fullfile(DATASET_DIR, 'IndexerData.dat');

addpath('util');
addpath(DATASET_DIR);