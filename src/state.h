/* discofs - disconnected file system
 * Copyright (c) 2012 Robin Martinjak
 * see LICENSE for full license (BSD 2-Clause)
 */

#ifndef DISCOFS_STATE_H
#define DISCOFS_STATE_H

enum states { STATE_ONLINE, STATE_OFFLINE, STATE_EXITING };

#define ONLINE  (state_get() == STATE_ONLINE)
#define OFFLINE (state_get() == STATE_OFFLINE)
#define EXITING (state_get() == STATE_EXITING)


int state_get();
void state_set(int state, int *oldstate);

#endif
