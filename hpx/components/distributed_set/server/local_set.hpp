//  Copyright (c) 2009-2010 Dylan Stark
// 
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#if !defined(HPX_COMPONENTS_SERVER_LOCAL_SET_AUG_13_2009_1054PM)
#define HPX_COMPONENTS_SERVER_LOCAL_SET_AUG_13_2009_1054PM

#include <iostream>

#include <hpx/hpx_fwd.hpp>
#include <hpx/runtime/applier/applier.hpp>
#include <hpx/runtime/threads/thread.hpp>
#include <hpx/runtime/components/component_type.hpp>
#include <hpx/runtime/components/server/simple_component_base.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace hpx { namespace components { namespace server
{
    ///////////////////////////////////////////////////////////////////////////
    /// The local_set is an HPX component.
    ///
    template <typename List>
    class HPX_COMPONENT_EXPORT local_set
      : public simple_component_base<local_set<List> >
    {
    private:
        typedef simple_component_base<local_set> base_type;
        
    public:
        local_set();
        
        //typedef local_set::server::local_set wrapping_type;
        typedef hpx::components::server::local_set<List> wrapping_type;
        
        enum actions
        {
            local_set_append = 0,
            local_set_get = 1
        };
        
        ///////////////////////////////////////////////////////////////////////
        // exposed functionality of this component

        typedef List list_type;

        int append(List);

        List get(void);

        ///////////////////////////////////////////////////////////////////////
        // Each of the exposed functions needs to be encapsulated into an action
        // type, allowing to generate all required boilerplate code for threads,
        // serialization, etc.
        typedef hpx::actions::result_action1<
            local_set, int, local_set_append, List, &local_set::append
        > append_action;

        typedef hpx::actions::result_action0<
            local_set, List, local_set_get, &local_set::get
        > get_action;

    private:
        List local_set_;
    };

}}}

#endif
