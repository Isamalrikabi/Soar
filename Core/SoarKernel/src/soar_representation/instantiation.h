/*************************************************************************
 * PLEASE SEE THE FILE "license.txt" (INCLUDED WITH THIS SOFTWARE PACKAGE)
 * FOR LICENSE AND COPYRIGHT INFORMATION.
 *************************************************************************/

/* instantiations.h */

#ifndef INSTANTIATIONS_H
#define INSTANTIATIONS_H

#include "kernel.h"
#include "stl_typedefs.h"

typedef struct pi_struct
{
    struct pi_struct* next, *prev;
    instantiation* inst;
} parent_inst;

typedef struct instantiation_struct
{
    struct production_struct*       prod;                   /* used full name of struct because
                                                               a forward declaration is needed -ajc (5/1/02) */
    struct instantiation_struct*    next, *prev;            /* dll of inst's from same prod */
    struct token_struct*            rete_token;             /* used by Rete for retractions */
    wme*                            rete_wme;               /* ditto */
    condition*                      top_of_instantiated_conditions;
    condition*                      bottom_of_instantiated_conditions;
    preference*                     preferences_generated;  /* prefs created by instantiation that are still in WM*/
    preference*                     preferences_cached;     /* shallow copies of retracted prefs for explainer*/
    Symbol*                         match_goal;             /* symbol, or NIL if none */
    goal_stack_level                match_goal_level;       /* level, or ATTRIBUTE_IMPASSE_LEVEL */
    bool                            tested_quiescence;
    bool                            tested_local_negation;
    bool                            tested_LTM;
    bool                            creates_deep_copy;
    bool                            in_ms;                  /* true iff this inst. is still in the match set */
    bool                            in_newly_created;       /* true iff this inst. is in the newly_created_instantiation list*/
    bool                            in_newly_deleted;       /* true iff this inst. is in the newly_deleted_instantiation list*/
    tc_number                       backtrace_number;
    bool                            GDS_evaluated_already;
    uint64_t                        i_id;                   /* id number used by EBC */
    Symbol*                         prod_name;
    tc_number                       explain_tc_num;
    EBCExplainStatus                explain_status;
    uint64_t                        explain_depth;
} instantiation;

void                init_instantiation_pool(agent* thisAgent);
void                init_instantiation(agent* thisAgent, instantiation* &inst, Symbol* backup_name, production* prod = NULL, struct token_struct* tok = NULL, wme* w = NULL);
void                create_instantiation(agent* thisAgent, production* prod, struct token_struct* tok, wme* w);
void                finalize_instantiation(agent* thisAgent, instantiation* inst, bool need_to_do_support_calculations, instantiation* original_inst);
void                retract_instantiation(agent* thisAgent, instantiation* inst);
void                deallocate_instantiation(agent* thisAgent, instantiation*& inst);

goal_stack_level    get_match_goal(condition* top_cond);
preference*         find_clone_for_level(preference* p, goal_stack_level level);
void                copy_OSK(agent* thisAgent, instantiation* inst);
Symbol*             instantiate_rhs_value(agent* thisAgent, rhs_value rv, goal_stack_level new_id_level, char new_id_letter, struct token_struct* tok, wme* w, bool& wasUnboundVar);

inline void         possibly_deallocate_instantiation(agent* thisAgent, instantiation* inst)
{
    if ((!(inst)->preferences_generated) && (!(inst)->in_ms)) deallocate_instantiation(thisAgent, inst);
}

instantiation*      make_architectural_instantiation(agent* thisAgent, Symbol* state, wme_set* conditions, symbol_triple_list* actions);
preference*         make_architectural_instantiation_for_impasse_item(agent* thisAgent, Symbol* goal, preference* cand);

/* -------------------------------------------------------------------
                              Instantiations

   Instantiations record two main things:
     (1) the instantiated LHS of the production,
     (2) the still-existing preferences that were generated by the RHS.

   Fields in an instantiation:

      prod:  points to the production.  (Note: this can also be NIL, for
        fake instantiations used for goal ^item augmentations.  See decide.c.)

      next, prev:  used for a doubly-linked list of instantiations of this
        production that are still in the match set.

      rete_token, rete_wme:  these fields are reserved for use by the Rete.
        (The Rete needs them to find the proper instantiation to retract
        when a token is delted from a p_node.)

      top_of_instantiated_conditions, bottom_of_instantiated_conditions:
        point to the top and bottom of the instantiated LHS conditions.

      preferences_generated:  header for a doubly-linked list of existing
        preferences that were created by this instantiation.

      match_goal:  points to the match goal of the instantiation, or NIL
        if there is none.

      match_goal_level:  goal stack level of the match goal, or
        ATTRIBUTE_IMPASSE_LEVEL if there is no match goal.

      reliable:  false iff instantiation is a justification whose
        backtrace either:

        - tests ^quiescence t, or
        - contains a local negated condition and learn -N is set, or
        - goes through an unreliable justification

        Intuitively, a justification is unreliable if its creation is
        not guaranteed by the state of production and working memory

      in_ms:  true iff this instantiation is still in the match set (i.e.,
        Rete-supported).
      backtrace_number:  used by the chunker to avoid backtracing through
        the same instantiation twice during the building of the same chunk.

      GDS_evaluated_already:  Most productions produce several actions.
        When we compute the goal-dependency-set (gds) gds for one wme of an
        instantiation, there's no point in redoing the work for a second wme
        from the same instantiation since the gds will be the same.  By
        testing this flag, we avoid duplicating this work.  The value is set
        to false whenever an instantiation is created.

   Reference counts on instantiations:
      +1 if it's in the match set
      +1 for each preference it created that's still around
   The reference count is kept implicitly using the preferences_generated
   and in_ms fields.  We deallocate an instantiation if its reference count
   goes to 0.
------------------------------------------------------------------- */
#endif
