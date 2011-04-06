% Copyright 2011 Zdenek Kalal
%
% This file is part of TLD.
% 
% TLD is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
% 
% TLD is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
% 
% You should have received a copy of the GNU General Public License
% along with TLD.  If not, see <http://www.gnu.org/licenses/>.

function [conf1,conf2,isin] = tldNN(x,tld)
% 'conf1' ... full model (Relative Similarity)
% 'conf2' ... validated part of model (Conservative Similarity)
% 'isnin' ... inside positive ball, id positive ball, inside negative ball

isin = nan(3,size(x,2));

if isempty(tld.pex) % IF positive examples in the model are not defined THEN everything is negative
    conf1 = zeros(1,size(x,2));
    conf2 = zeros(1,size(x,2));
    return;
end

if isempty(tld.nex) % IF negative examples in the model are not defined THEN everything is positive
    conf1 = ones(1,size(x,2));
    conf2 = ones(1,size(x,2));
    return;
end

conf1 = nan(1,size(x,2));
conf2 = nan(1,size(x,2));

for i = 1:size(x,2) % fore every patch that is tested
    
    nccP = distance(x(:,i),tld.pex,1); % measure NCC to positive examples
    nccN = distance(x(:,i),tld.nex,1); % measure NCC to negative examples
    
    % set isin
    if any(nccP > tld.model.ncc_thesame), isin(1,i) = 1;  end % IF the query patch is highly correlated with any positive patch in the model THEN it is considered to be one of them
    [~,isin(2,i)] = max(nccP); % get the index of the maximall correlated positive patch
    if any(nccN > tld.model.ncc_thesame), isin(3,i) = 1;  end % IF the query patch is highly correlated with any negative patch in the model THEN it is considered to be one of them
    
    % measure Relative Similarity
    dN = 1 - max(nccN);
    dP = 1 - max(nccP);
    conf1(i) = dN / (dN + dP);

    % measure Conservative Similarity
    maxP = max(nccP(1:ceil(tld.model.valid*size(tld.pex,2))));
    dP = 1 - maxP;
    conf2(i) = dN / (dN + dP);
    
end