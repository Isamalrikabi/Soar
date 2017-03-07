/*
 * ebc_identity_set.h
 *
 *  Created on: Mar 4, 2017
 *      Author: mazzin
 */
#ifndef CORE_SOARKERNEL_SRC_EXPLANATION_BASED_CHUNKING_EBC_IDENTITY_SET_H_
#define CORE_SOARKERNEL_SRC_EXPLANATION_BASED_CHUNKING_EBC_IDENTITY_SET_H_

#include "kernel.h"

#include "dprint.h"
#include "stl_typedefs.h"

class IdentitySet
{
    public:

        IdentitySet() {};
        ~IdentitySet() {};

        void init(agent* my_agent);
        void clean_up();

        void        add_ref()               { ++refcount; dprint(DT_DEBUG, "++ identity set %u --> %u.\n", idset_id, refcount); }
        bool        remove_ref()            { --refcount; return (refcount == 0);}

        bool        literalized()           { return super_join->m_literalized; }
        bool        joined()                { return (super_join != this); }
        uint64_t    get_identity()          { return super_join->idset_id; }
        uint64_t    get_sub_identity()      { return idset_id; }
        uint64_t    get_clone_identity()    { return super_join->clone_identity; }
        Symbol*     get_var()               { return super_join->new_var; }
        condition*  get_operational_cond()  { return super_join->operational_cond; }
        WME_Field   get_operational_field() { return super_join->operational_field; }
        uint64_t    get_refcount()          { return refcount; }

        void literalize()                               { super_join->m_literalized = true; super_join->touch(); }
        void set_clone_identity(uint64_t pID)           { super_join->clone_identity = pID; super_join->touch(); }
        void set_var(Symbol* pVar)                      { super_join->new_var = pVar; super_join->touch(); }

        void set_operational_cond(condition* pCond, WME_Field pField);
        void store_variablization(Symbol* variable, Symbol* pMatched_sym);
        void update_clone_id();
        void touch();
        void clean_up_transient();

        /* An ID used for printing and to make debugging easier. Not used by identity analysis logic. */
        uint64_t                    idset_id;

        /* A pointer to the join super identity.  Used to point back to itself but that's bad with shared pointers */
        IdentitySetSharedPtr        super_join;

        /* Pointers to other identity_set nodes that map to this one */
        identity_set_list*          identity_sets;

        /* Flag indicating whether any transient data has been changed */
        bool                        dirty;

    private:

        agent*                      thisAgent;

        /* Fields for variablization and chunk instantiation identity creation */
        Symbol*                     new_var;
        uint64_t                    clone_identity;
        bool                        m_literalized;

        /* Fields for transitive constraint attachment  */
        condition*                  operational_cond;
        WME_Field                   operational_field;

        uint64_t                    refcount;

};

#endif /* CORE_SOARKERNEL_SRC_EXPLANATION_BASED_CHUNKING_EBC_IDENTITY_SET_H_ */
