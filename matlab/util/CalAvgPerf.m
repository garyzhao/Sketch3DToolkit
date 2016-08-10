%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% calculate average performance, the micro average will be calculated.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [Precision] = CalAvgPerf(P, C, size, file)

CUTOFF = 2;
SAMPLE = 20;
mean = zeros(1, SAMPLE);

for j = 1:SAMPLE
    valid = 0;
    for i = 1:size
        % only consider classes of a valid size and only average over real interpolated results this means avoid classes between precision 1 and 1/(classsize-1)
        if (C(i) < CUTOFF || C(i) < SAMPLE / j)
            continue;
        end
        [tmp] = InterpolatePerf(P(i, :), C(i), j / SAMPLE); %
        mean(1, j) = mean(1, j) + tmp;
        valid = valid + 1;
    end
    if (valid > 0)
        mean(1, j) = mean(1, j) / valid;
    end
end

fp = fopen(file, 'a');
for j = 1:SAMPLE
    fprintf(fp, '%f\t%f\n', j / SAMPLE, mean(1, j));
end
Precision = mean;
fclose(fp);
