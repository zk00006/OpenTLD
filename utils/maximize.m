function maximize(fig)
% MAXIMIZE Size a window to fill the entire screen.
%
% maximize(HANDLE fig)
%  Will size the window with handle fig such that it fills the entire screen.
%

% Modification History
% ??/??/2001  WHF  Created.
% 04/17/2003  WHF  Found 'outerposition' undocumented feature.
%

% Original author: Bill Finger, Creare Inc.
% Free for redistribution as long as credit comments are preserved.
%

if nargin==0, fig=gcf; end

units=get(fig,'units');
set(fig,'units','normalized','outerposition',[0 0 1 1]);
set(fig,'units',units);

% Old way:	
% These are magic numbers which are probably system dependent.
dim=get(0,'ScreenSize')-[0 -5 0 72];

set(fig,'Position',dim); 


