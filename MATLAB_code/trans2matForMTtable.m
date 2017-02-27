function [breakToffs, slopes, numValidData] = trans2matForMTtable(table)

nstages = numel(table);

numValidData = zeros(1, nstages);

for i = 1 : nstages
    numValidData(i) = size( table{i}, 1);
end

breakToffs = zeros(max(numValidData), nstages);
slopes = breakToffs;

for i = 1 : nstages
    breakToffs(1:numValidData(i), i) = table{i}(:,1);
    slopes(1:numValidData(i), i) = table{i}(:,2);
end
