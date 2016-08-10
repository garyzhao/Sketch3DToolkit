% Draw Precision-Recall Curves (PR Curves) for evaluated methods.
setupvars;
performance_data_list = { RANK_DIR, 'Ours';
    
    'Furuya (BF-fDSIFT)',                       'Furuya (BF-fDSIFT)';
    'Furuya (BF-fGALIF)',                       'Furuya (BF-fGALIF)';
    'Furuya (BF-fGALIF+BF-fDSIFT)',             'Furuya (BF-fGALIF+BF-fDSIFT)';
    'Furuya (CDMR-BF-fDSIFT)',                  'Furuya (CDMR-BF-fDSIFT)';
    'Furuya (CDMR-BF-fGALIF)',                  'Furuya (CDMR-BF-fGALIF)';
    'Furuya (CDMR-BF-fGALIF+CDMR-BF-fDSIFT)',   'Furuya (CDMR-BF-fGALIF+CDMR-BF-fDSIFT)';
    'Furuya (UMR-BF-fDSIFT)',                   'Furuya (UMR-BF-fDSIFT)';
    'Furuya (UMR-BF-fGALIF)',                   'Furuya (UMR-BF-fGALIF)';
    'Furuya (UMR-BF-fGALIF+UMR-BF-fDSIFT)',     'Furuya (UMR-BF-fGALIF+UMR-BF-fDSIFT)';
    'Li (SBR-2D-3D_NUM_50)',                    'Li (SBR-2D-3D\_NUM\_50)';
    'Li (SBR-VC_NUM_50)',                       'Li (SBR-VC\_NUM\_50)';
    'Li (SBR-VC_NUM_100)',                      'Li (SBR-VC\_NUM\_100)';
    'Saavedra (FDC)',                           'Saavedra (FDC)';
    'Saavedra (FDC-CVIU version)',              'Saavedra (FDC-CVIU version)';
    'Saavedra (HELO-SIL)',                      'Saavedra (HELO-SIL)';
    'Saavedra (HOG-SIL)',                       'Saavedra (HOG-SIL)';
    'Pascoal (HTD)',                            'Pascoal (HTD)';
    'Aono (EFSD)',                              'Aono (EFSD)';
};

figure('Position', [200, 200, 700, 700]);
hold on;
for i = 1:size(performance_data_list, 1)
    if i == 1
        fid = fopen(fullfile(performance_data_list{i, 1}, 'PR_test.txt'));
    else
        fid = fopen(fullfile(DATASET_PERFORMANCE_DATA_DIR, performance_data_list{i, 1}, 'PR_test.txt'));
    end
    
    fgetl(fid);
    PR = fscanf(fid, '%f\t%f', [2 Inf]);
    fclose(fid);
    
    disp(i);
    Recall = PR(1, 1:end);
    Precision = PR(2, 1:end);
    
    if i == 1
        plot(Recall, Precision, '--rs', 'linewidth', 1.8);
    else
        plot(Recall, Precision, '-o');
    end
    % plot(Recall, Precision, '*');
end
axis([0 1 0 0.7]);
grid on;
hold off;
xlabel('Recall'), ylabel('Precision');
legend(performance_data_list(:, 2));