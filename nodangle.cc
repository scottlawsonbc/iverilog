/*
 * Copyright (c) 1999Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */
#if !defined(WINNT)
#ident "$Id: nodangle.cc,v 1.2 1999/11/28 23:42:02 steve Exp $"
#endif

/*
 * This functor scans the design looking for dangling objects and
 * excess local signals. These deletions are not necessarily required
 * for proper functioning of anything, but they can clean up the
 * appearance of design files that are generated.
 */
# include  "functor.h"
# include  "netlist.h"

class nodangle_f  : public functor_t {
    public:
      void signal(Design*des, NetNet*sig);
};


void nodangle_f::signal(Design*des, NetNet*sig)
{
      if (! sig->local_flag())
	    return;
      if (sig->get_eref() > 0)
	    return;

	/* Check to see if there is some significant signal connected
	   to every pin of this signal. */
      unsigned significant_flags = 0;
      for (unsigned idx = 0 ;  idx < sig->pin_count() ;  idx += 1) {
	    NetObj::Link&lnk = sig->pin(idx);
	    for (NetObj::Link*cur = lnk.next_link()
		       ; cur != &lnk ;  cur = cur->next_link()) {
		  NetNet*cursig = dynamic_cast<NetNet*>(cur->get_obj());
		  if (cursig == 0)
			continue;
		  if (cursig->local_flag())
			continue;
		  significant_flags += 1;
		  break;
	    }
      }

	/* If every pin is connected to another significant signal,
	   then I can delete this one. */
      if (significant_flags == sig->pin_count())
	    delete sig;
}

void nodangle(Design*des)
{
      nodangle_f fun;
      des->functor(&fun);
}

/*
 * $Log: nodangle.cc,v $
 * Revision 1.2  1999/11/28 23:42:02  steve
 *  NetESignal object no longer need to be NetNode
 *  objects. Let them keep a pointer to NetNet objects.
 *
 * Revision 1.1  1999/11/18 03:52:20  steve
 *  Turn NetTmp objects into normal local NetNet objects,
 *  and add the nodangle functor to clean up the local
 *  symbols generated by elaboration and other steps.
 *
 */

