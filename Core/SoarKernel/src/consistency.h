/*************************************************************************
 * PLEASE SEE THE FILE "license.txt" (INCLUDED WITH THIS SOFTWARE PACKAGE)
 * FOR LICENSE AND COPYRIGHT INFORMATION.
 *************************************************************************/

/*************************************************************************
 *
 *  file:  consistency.h
 *
 * =======================================================================
 *
 * Source code for Operand2/Waterfall specific functions in the kernel.
 *
 * ======================================================================= */

#ifndef CONSISTENCY_H
#define CONSISTENCY_H

#include "gdatastructs.h"

typedef char Bool;
typedef signed short goal_stack_level;
typedef struct slot_struct slot;
typedef struct wme_struct wme;
typedef struct agent_struct agent;
typedef struct ms_change_struct ms_change;
typedef union symbol_union Symbol;

/* For information on the consistency check routines */
/* #define DEBUG_CONSISTENCY_CHECK    */

/* For information on aspects of determining the active level */
/* #define DEBUG_DETERMINE_LEVEL_PHASE   */

#define NEW_DECISION         0
#define SAME_LEVEL           1
#define HIGHER_LEVEL         2
#define LOWER_LEVEL          3
#define NIL_GOAL_RETRACTIONS 4

void remove_operator_if_necessary(agent* thisAgent, slot *s, wme *w);
Bool decision_consistent_with_current_preferences(agent* thisAgent, Symbol *goal, slot *s);
void remove_current_decision(agent* thisAgent, slot *s);
Bool check_context_slot_decisions (agent* thisAgent, goal_stack_level level);

/* REW: begin 08.20.97 */  /* To implement the Waterfall part of Operand2 */
extern void print_assertion(ms_change *msc);
extern void print_retraction(ms_change *msc);
void initialize_consistency_calculations_for_new_decision(agent* thisAgent);
void determine_highest_active_production_level_in_stack_apply(agent* thisAgent);
void determine_highest_active_production_level_in_stack_propose(agent* thisAgent);
Bool goal_stack_consistent_through_goal(agent* thisAgent, Symbol *goal);
Bool i_activity_at_goal(Symbol *goal);
Bool minor_quiescence_at_goal(agent* thisAgent, Symbol *goal);
int active_production_type_at_goal(Symbol *goal);

Symbol * highest_active_goal_propose(agent* thisAgent, Symbol* start_goal, Bool noneOk);
Symbol * highest_active_goal_apply(agent* thisAgent, Symbol* start_goal, Bool noneOk);

/* REW: end   08.20.97 */

#endif
