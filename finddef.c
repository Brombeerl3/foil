/******************************************************************************/
/*									      */
/*	All the stuff for trying possible next literals, growing clauses      */
/*	and assembling definitions					      */
/*									      */
/******************************************************************************/


#include  "defns.i"
#include  "extern.i"


int	FalsePositive,
	FalseNegative;


void  FindDefinition(Relation R)
/*    --------------  */
{
    int Size;

    Target = R;
    NCl = 0;

    printf("\n----------\n%s:\n", R->Name);

    OriginalState(R);

    /*  Make own copy of tuple set  */

    StartClause = StartDef;

    Size = StartDef.NTot+1;
    StartClause.Tuples = Alloc(Size, Tuple);
    memcpy(StartClause.Tuples, StartDef.Tuples, Size*sizeof(Tuple));

    NRecLitDef = 0;

    FalsePositive = 0;
    FalseNegative = StartDef.NPos;

    R->Def = Alloc(100, Clause);

    while ( StartClause.NPos )
    { 
	if ( ! (R->Def[NCl] = FindClause()) ) break;

	R->Def[NCl++][NLit] = Nil;

	if ( NCl % 100 == 0 )
	{
	    Realloc(R->Def, NCl+100, Clause);
	}

	NRecLitDef += NRecLitClause;
    }
    R->Def[NCl] = Nil;

    SiftClauses();

    if ( FalsePositive || FalseNegative )
    {
	printf("\n***  Warning: the following definition\n");

	if ( FalsePositive )
	{
	    printf("***  matches %d tuple%s not in the relation\n",
		FalsePositive, Plural(FalsePositive));
	}

	if ( FalseNegative )
	{
	    printf("***  does not cover %d tuple%s in the relation\n",
		FalseNegative, Plural(FalseNegative));
	}
    }

    PrintDefinition(R);

    pfree(StartClause.Tuples);
    pfree(StartDef.Tuples);
}



Clause  FindClause()
/*      ----------  */
{
    Tuple Case, *TSP;

    InitialiseClauseInfo();

    GrowNewClause();

    NewClause[NLit] = Nil;
    if ( NLit > 1 ) PruneNewClause();

    /*  Make sure accuracy criterion is satisfied  */

    if ( ! NLit || Current.NOrigPos < MINACCURACY * Current.NOrigTot )
    {
	if ( NLit )
	{
	    Verbose(1)
		printf("\nClause too inaccurate (%d/%d)\n",
		       Current.NOrigPos, Current.NOrigTot);
	}

	pfree(NewClause);
	return Nil;
    }

    FalsePositive += Current.NOrigTot - Current.NOrigPos;
    FalseNegative -= Current.NOrigPos;

    /*  Set flags for positive covered tuples  */

    ClearFlags;

    for ( TSP = Current.Tuples ; Case = *TSP ; TSP++ )
    {
	if ( Positive(Case) )
	{
	    SetFlag(Case[0]&Mask, TrueBit);
	}
    }

    if ( Current.Tuples != StartClause.Tuples )
    {
	FreeTuples(Current.Tuples, true);
    }

    /*  Copy all negative tuples and uncovered positive tuples  */

    StartClause.NTot = StartClause.NPos = 0;

    for ( TSP = StartClause.Tuples ; Case = *TSP ; TSP++ )
    {
	if ( ! Positive(Case) || ! TestFlag(Case[0]&Mask, TrueBit) )
	{
	    StartClause.Tuples[StartClause.NTot++] = Case;
	    if ( Positive(Case) ) StartClause.NPos++;
	}
    }
    StartClause.Tuples[StartClause.NTot] = Nil;

    StartClause.NOrigPos = StartClause.NPos;
    StartClause.NOrigTot = StartClause.NTot;

    StartClause.BaseInfo = Info(StartClause.NPos, StartClause.NTot);
    Current = StartClause;

    Verbose(1)
    {
	printf("\nClause %d: ", NCl);
	PrintClause(Target, NewClause);
    }

    return NewClause;
}



void  ExamineLiterals()
/*    ---------------  */
{
    Relation	R;
    int		i, Relns=0;

    NPossible = NDeterminate = 0;

    MaxPossibleGain = Current.NPos * Current.BaseInfo;

    /*  If this is not the first literal, review coverage and check
	variable orderings, identical variables etc.  */

    if ( NLit != 0 )
    {
	CheckOriginalCaseCover();
	ExamineVariableRelationships();
    }

    AvailableBits = Except(StartClause.NTot, Current.NOrigPos) - ClauseBits;

    Verbose(1)
    {
	printf("\nState (%d/%d, %.1f bits available",
	       Current.NPos, Current.NTot, AvailableBits);

	if ( NWeakLits )
	{
	    Verbose(2)
		printf(", %d weak literal%s", NWeakLits, Plural(NWeakLits));
	}
	printf(")\n");

	Verbose(4)
	    PrintTuples(Current.Tuples, Current.MaxVar);

	putchar('\n');
    }

    /*  Find possible literals for each relation  */

    ForEach(i, 0, MaxRel)
    {
	R = RelnOrder[i];

	ExploreArgs(R, true);

	if ( R->NTrialArgs ) Relns++;
    }

    /*  Evaluate them  */

    AlterSavedClause = Nil;

    for ( i = 0 ; i <= MaxRel && BestLitGain < MaxPossibleGain ; i++ )
    {
	R = RelnOrder[i];

	R->Bits = Log2(Relns) + Log2(R->NTrialArgs+1E-3)
			      - Log2(NLit+1.0001-NDetLits);
	if ( NEGLITERALS || Predefined(R) ) R->Bits += 1.0;
	if ( R == GTCONST ) R->Bits += Log2(Current.NTot);

	if ( R->Bits > AvailableBits )
	{
	    Verbose(2)
	    {
		printf("\t\t\t\t[%s requires %.1f bits]\n", R->Name, R->Bits);
	    }
	}
	else
	{
	    ExploreArgs(R, false);

	    /*if ( R->NTrialArgs )*/
	    {
		Verbose(2)
		    printf("\t\t\t\t[%s tried %d/%d] %.1f secs\n",
			   R->Name, R->NTried, R->NTrialArgs, CPUTime());
	    }
	}
    }
}



void  GrowNewClause()
/*    -------------                */
{
    Literal	L;
    int		i, OldNLit, SavedClauseLength;
    Boolean	Progress=true;
    float	Accuracy;

    while ( Progress && Current.NPos < Current.NTot )
    {
	ExamineLiterals();

	/*  If have noted better saveable clause, record it  */

	if ( BetterSaveableClause() )
	{
	    Realloc(SavedClause, NLit+2, Literal);
	    ForEach(i, 0, NLit-1)
	    {
		SavedClause[i] = NewClause[i];
	    }
	    SavedClause[NLit]   = AllocZero(1, struct _lit_rec);
	    SavedClause[NLit+1] = Nil;

	    SavedClause[NLit]->Rel      = AlterSavedClause->Rel;
	    SavedClause[NLit]->Sign     = AlterSavedClause->Sign;
	    SavedClause[NLit]->Args     = AlterSavedClause->Args;
	    SavedClause[NLit]->Bits     = AlterSavedClause->Bits;
	    SavedClause[NLit]->WeakLits = 0;

	    SavedClauseCover    = AlterSavedClause->PosCov;
	    SavedClauseAccuracy = AlterSavedClause->PosCov /
					  (float) AlterSavedClause->TotCov;
	    SavedClauseLength = NLit+1;

	    Verbose(1)
	    {
		printf("\t\tSave clause ending with ");
		PrintLiteral(SavedClause[NLit]);
		printf(" (cover %d, accuracy %d%%)\n",
		       SavedClauseCover, (int) (100*SavedClauseAccuracy));
	    }

	    pfree(AlterSavedClause);
	}

	if ( NDeterminate && BestLitGain < DETERMINATE * MaxPossibleGain )
	{
	    ProcessDeterminateLiterals(true);
	}
	else
	if ( NPossible )
	{
	    /*  At least one gainful literal  */

	    NewClause[NLit++] = L = SelectLiteral();
	    ClauseBits += L->Bits;
	    if ( NLit % 100 == 0 ) Realloc(NewClause, NLit+100, Literal);

	    Verbose(1)
	    {
		printf("\nBest literal ");
		PrintLiteral(L);
		printf(" (%.1f bits)\n", L->Bits);
	    }

	    /*  Check whether should regrow clause  */

	    if ( L->Rel != Target && AllLHSVars(L) &&
		 Current.MaxVar > Target->Arity && ! AllDeterminate() )
	    {
		Verbose(1) printf("\tRegrowing clause\n");

		OldNLit = NLit;
		NLit = 0;
		ForEach(i, 0, OldNLit-1)
		{
		    if ( AllLHSVars(NewClause[i]) )
		    {
			NewClause[NLit++] = NewClause[i];
		    }
		}
		NewClause[NLit] = Nil;

		RecoverState(NewClause);
		GrowNewClause();
		return;
	    }

	    NWeakLits = L->WeakLits;

	    if ( L->Rel == Target ) AddOrders(L);

	    NewState(L, Current.NTot);

	    if ( L->Rel == Target ) NoteRecursiveLit(L);
	}
	else
	{
	    Verbose(1) printf("\nNo literals\n");

	    Progress = Recover();
	}
    }

    /*  Finally, see whether saved clause is better  */

    CheckOriginalCaseCover();
    Accuracy = Current.NOrigPos / (float) Current.NOrigTot;
    if ( SavedClause &&
	 ( SavedClauseAccuracy > Accuracy ||
	   SavedClauseAccuracy == Accuracy &&
	   SavedClauseCover > Current.NOrigPos ||
	   SavedClauseAccuracy == Accuracy &&
	   SavedClauseCover == Current.NOrigPos &&
	   SavedClauseLength < NLit ) )
    {
	Verbose(1) printf("\tReplace by saved clause\n");
	RecoverState(SavedClause);
	CheckOriginalCaseCover();
    }
}


    InitialiseClauseInfo()
/*  --------------------  */
{
    Var V;

    /*  Initialise everything for start of new clause  */

    NewClause = Alloc(100, Literal);

    Current = StartClause;

    NLit = NDetLits = NWeakLits = NRecLitClause = 0;

    NToBeTried = 0;
    AnyPartialOrder = false;

    AvailableBits = Except(StartClause.NTot, StartClause.NPos);
    ClauseBits = 0;

    ForEach(V, 1, Target->Arity)
    {
	Variable[V]->Depth   = 0;
	Variable[V]->Type    = Target->Type[V];
	Variable[V]->TypeRef = Target->TypeRef[V];
    }

    memset(Barred, false, MAXVARS+1);

    SavedClause = Nil;
    SavedClauseAccuracy = 0;
}



	/*  See whether we now have a better clause to save, i.e.
	    AlterSavedClause exists and the literal is not going to be
	    put on the current clause anyway  */

#define	Next	Possible[1]


Boolean  BetterSaveableClause()
/*       --------------------  */
{
    Literal	L;
    int		i;

    if ( ! AlterSavedClause ) return false;

    if ( NDeterminate && BestLitGain < DETERMINATE * MaxPossibleGain )
    {
	ForEach(i, 0, NDeterminate-1)
	{
	    L = NewClause[NLit+i];

	    if ( SameLiteral(L->Rel, L->Sign, L->Args) ) return false;
	}

	return true;
    }
    else
    {
	return ! SameLiteral(Next->Rel, Next->Sign, Next->Args);
    }
}



Boolean  SameLiteral(Relation R, Boolean Sign, Var *A)
/*       -----------  */
{
    Var V, NArgs;

    if ( R != AlterSavedClause->Rel || Sign != AlterSavedClause->Sign )
    {
	return false;
    }

    NArgs = ( HasConst(R) ? 1 + sizeof(Const) : R->Arity);
    ForEach(V, 1, NArgs)
    {
	if ( A[V] != AlterSavedClause->Args[V] ) return false;
    }

    return true;
}



Boolean  AllLHSVars(Literal L)
/*       ----------  */
{
    Var V;

    ForEach(V, 1, L->Rel->Arity)
    {
	if ( L->Args[V] > Target->Arity ) return false;
    }

    return true;
}



	/*  See whether all literals in clause are determinate  */

Boolean  AllDeterminate()
/*       --------------  */
{
    int i;

    ForEach(i, 0, NLit-2)
    {
	if ( NewClause[i]->Sign != 2 ) return false;
    }

    return true;
}
