function [] = saveCoolingData(prefix, table)

[breakToffs, slopes, numValidData] = trans2matForMTtable(table);

csvwrite([prefix, '_coolBreakToffs.csv'], breakToffs);
csvwrite([prefix, '_coolslopes.csv'], slopes);
csvwrite([prefix, '_numValidData.csv'], numValidData);

end