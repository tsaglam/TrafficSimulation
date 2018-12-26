# To Do

## Open Questions

 * How to properly initialise and manage TrafficLightSignaler.

   Currently: Completely managed by LowLevelStreet, both are initialised
   together by ModelSyncer. Where do parameters for the traffic light car come
   from?

 * What happens when there is no next / previous car?

   The problem is trivially solvable when using iterators through the use of
   end() (past the end) iterators. Dereferencing the end iterator is undefined
   behaviour.

   This poses a problem when using a reference to return the car. This problem
   can be avoided by returning a pointer instead of a reference. How is "is
   end?" checked for the iterator? By defining that the iterators supplied to
   and returned by getNextCar() / getPrevCar() are iterators of the
   allIterable(), the implementation could check equality to the end()
   iterator.

 * Restructuring iterables of RfbStructure.

   Currently, there are two iterables retrievable via method calls, one for
   all vehicles and one for vehicle beyond the end of the street. The iterator
   methods of the allIterable could be moved to the RfbStructure class to
   simplify the most common access operations.

   This would result in the "no next / previous car" problem described above
   to be more logically consistent.

 * Traffic signal car: Only one per street or one per lane?

   There is only a single traffic signal car per street with lane = 0 in the
   current design. Is this sufficient or is it required for the traffic signal
   car to have the correct lane set? In this case multiple traffic signal cars
   per street are required.

   Multiple cars per street could be implemented using an internal array with
   a fixed number of fields. There is an upper bound on the number of lanes
   per street (3). This would be used as the size of the array.

 * Computation Routines in Simulator: When are instances initialised?

   Instances may be initialised upon construction of the Simulator instance.
   Alternatively, instances may be initialised when first required and stored
   in members (operator=) or only temporary (stack) during computeStep().
   Does this help with avoiding circular template definitions?
