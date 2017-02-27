function y = functemplate(x, parameter)
if any(x <= 0)
    y = inf(size(x));
    return;
end

% parameter(2:end, 2) = ( parameter(2:end, 3) - parameter(1:end-1, 3) ) ./ ...
%     ( parameter(2:end, 1) - parameter(1:end-1, 1) );
allx = parameter(:,1);
numallx = numel(allx);
numx = numel(x);
segId = zeros(numx, 1);
for i = 1 : numx
    segId(i) = find( x(i) <= allx, 1, 'first');
    if isempty(segId(i))
       segId(i) = numallx;
    end
end

y = parameter(segId, 3) + parameter(segId, 2).*( x(:) - parameter(segId, 1));


end
