%% FOIL 6.0   [October 1993]
%% --------

%% Relation *row

row([A,B,C]) :-
    all_different([A,B,C]).

%% Relation cols
%%     (warning: contains duplicate positive tuples)

:- use_module(library(clpfd)).

cols([],[],[]).
cols([R1|R1s],[R2|R2s],[R3|R3s]) :-
    all_different([R1,R2,R3]),
    cols(R1s,R2s,R3s).

%% Relation sudoku (FOIL generated)

sudoku_3x3(A,B,C) :-
    cols(A,B,C),
    row(A),
    row(B).

:- sudoku_3x3([1,2,3],[2,3,1],[3,1,2]).
:- \+ sudoku_3x3([1,1,1],[1,1,1],[1,1,1]).

test([A1,A2,A3],[B1,B2,B3],[C1,C2,C3]) :-
    [A1,A2,A3,B1,B2,B3,C1,C2,C3] ins 1..3,
    sudoku_3x3([A1,A2,A3],[B1,B2,B3],[C1,C2,C3]),
    label([A1,A2,A3,B1,B2,B3,C1,C2,C3]).

% sudoku_3x3 completely specified
:- \+ (test(_As,_Bs,Cs), \+ row(Cs)).

% sudoku w/o clp

members([X|Xs],Ys) :- member(X,Ys), members(Xs,Ys).
members([],_Ys).

permutation(Xs,[Z|Zs]) :- select(Z,Xs,Ys), permutation(Ys,Zs).
permutation([],[]).

sudoku(Rows) :-
	flatten(Rows,Xs),
	members([1,2,3,4,5,6,7,8,9],Xs).
	rows(Rows),
	cols(Rows),
	blocks(Rows).

rows([]).
rows([R|Rs]) :-
    permutation(R,[1,2,3,4,5,6,7,8,9]),
    rows(Rs).

cols([[]|_]).
cols([
      [X1|R1],
      [X2|R2],
      [X3|R3],
      [X4|R4],
      [X5|R5],
      [X6|R6],
      [X7|R7],
      [X8|R8],
      [X9|R9]]) :-
	permutation([X1,X2,X3,X4,X5,X6,X7,X8,X9],[1,2,3,4,5,6,7,8,9]), 
	cols([R1,R2,R3,R4,R5,R6,R7,R8,R9]).

blocks([]).
blocks([[],[],[]|Rs]) :- blocks(Rs).
blocks([[X1,X2,X3|R1],
	[X4,X5,X6|R2],
	[X7,X8,X9|R3] |Rs]) :- 
	permutation([X1,X2,X3,X4,X5,X6,X7,X8,X9],[1,2,3,4,5,6,7,8,9]), 
	blocks([R1,R2,R3|Rs]).

problem(0,[[_,_,_,_,_,_,_,_,_],
	   [_,_,_,_,_,_,_,_,_],
	   [_,_,_,_,_,_,_,_,_],
	   [_,_,_,_,_,_,_,_,_],
	   [_,_,_,_,_,_,_,_,_],
	   [_,_,_,_,_,_,_,_,_],
	   [_,_,_,_,_,_,_,_,_],
	   [_,_,_,_,_,_,_,_,_],
	   [_,_,_,_,_,_,_,_,_]]).
problem(1,[[1,_,_,_,_,_,_,_,_],
           [_,_,2,7,4,_,_,_,_],
           [_,_,_,5,_,_,_,_,4],
           [_,3,_,_,_,_,_,_,_],
           [7,5,_,_,_,_,_,_,_],
           [_,_,_,_,_,9,6,_,_],
           [_,4,_,_,_,6,_,_,_],
           [_,_,_,_,_,_,_,7,1],
           [_,_,_,_,_,1,_,3,_]]).
problem(2,[[_,6,_,1,_,4,_,5,_],
	   [_,_,8,3,_,5,6,_,_],
	   [2,_,_,_,_,_,_,_,1],
	   [8,_,_,4,_,7,_,_,6],
	   [_,_,6,_,_,_,3,_,_],
	   [7,_,_,9,_,1,_,_,4],
	   [5,_,_,_,_,_,_,_,2],
	   [_,_,7,2,_,6,9,_,_],
	   [_,4,_,5,_,8,_,7,_]]).

solve(N) :-
    problem(N,P),
    sudoku(P).
