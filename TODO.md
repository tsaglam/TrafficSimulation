# To Do

## Open Questions

 * How to properly initialise and manage TrafficLightSignaler.

   Currently: Completely managed by LowLevelStreet, both are initialised
   together by ModelSyncer. Where do parameters for the traffic light car come
   from?

   In discussion: ModelSyncer handles all details required for the lowlevel
   module which are not specified as part of classes managed by the domain
   model. This make it a good fit for initialising the traffic light signaler
   including passing the parameters for the traffic light car.

   **Status**: Consent Reached in Discussion

 * What happens when there is no next / previous car?

   The problem is trivially solvable when using iterators through the use of
   end() (past the end) iterators. Dereferencing the end iterator is undefined
   behaviour. By defining that the iterators supplied to and returned by
   getNextCar() / getPrevCar() are iterators of the allIterable() iterable,
   the implementation could check equality to the end() iterator.

   Alternatively, an additional iterator state "no car" could be defined. An
   iterator with this state is returned by getNextCar() / getPrevCar() when
   there is no next / previous car. An iterator of this state can be
   constructed by third-party types using an additional method on the
   allIterable() iterable.

   In discussion: An iterator in the "end" state seems sufficient. This
   iterator is be == equal to end().

   **Status**: Consent Reached in Discussion

 * Restructuring iterables of RfbStructure.

   Currently, there are two iterables retrievable via method calls, one for
   all vehicles and one for vehicle beyond the end of the street. The iterator
   methods of the "all" iterable could be moved to the RfbStructure class to
   simplify the most common access operations.

   This would result in the "which end() iterator to use?" problem described
   in the previous point to be more logically consistent.

   In discussion: Don't change, allIterable() has to be used to obtain an
   iterable for all vehicles on the street.

   **Status**: Consent Reached in Discussion

 * How to return the traffic light car?

   The TrafficLightSignaler proxies access to the RfbStructure and returns the
   TrafficLightCar when applicable. Returning an iterator is problematic, as
   the TrafficLightCar is not tracked by the RfbStructure.

   Instead, a pointer to a car could be returned. The iterator returned by the
   RfbStructure is dereferenced and returned; if applicable a pointer to the
   traffic light car is returned. If there is no next or previous car the
   nilptr is returned (checked by comparing to end()).

   This poses a problem for the computation routines: In many cases it is
   required to retrieve the next / previous car of the returned car. A
   solution: The RfbStructure interface is extended by adding an
   "out-of-street" state to the iterator. This state may be initialised by
   third-party types with a reference to an arbitrary car (returned upon
   dereferencing). An iterator in the "out-of-street" state is invalid to be
   passed to getNextCar() / getPrevCar().

   In discussion: An independent iterator is to be defined as part of the
   TrafficLightSignaler header file. This iterator wraps iterators defined in
   RfbStructure implementations and provides full semantic support for traffic
   light cars: Retrieving the car in front or in back of the traffic light car
   will return the expected result. The implementation of this iterator is
   non-trivial, some aspects to consider: Equality checks (operator==), end()
   iterator, dereferencing.

   **Status**: Consent Reached in Discussion

 * Traffic signal car: Only one per street or one per lane?

   There is only a single traffic signal car per street with lane = 0 in the
   current design. Is this sufficient or is it required for the traffic signal
   car to have the correct lane set? In this case multiple traffic signal cars
   per street are required.

   Multiple cars per street could be implemented using an internal array with
   a fixed number of fields. There is an upper bound on the number of lanes
   per street (3). This would be used as the size of the array.

   In discussion: The lane of the traffic light car is never retrieved. Hence,
   having a single traffic light car per street seems sufficient. This car has
   its lane attribute set to 0.

   **Status**: Consent Reached in Discussion

 * Computation Routines in Simulator: When are instances initialised?

   Instances may be initialised upon construction of the Simulator instance.
   Alternatively, instances may be initialised when first required and stored
   in members (operator=) or only temporary (stack) during computeStep().
   Does this help with avoiding circular template definitions?

   The most obvious solution is to solve circular template definitions by
   extracting base types and initialising computation routines upon
   construction of the Simulator instance.

   In discussion: Computation routines are initialised upon initialisation of
   the Simulator. They are passed a reference to the simulation data. The
   reference is used to access and modify any data during computation. The
   single computation routine instance persists until the Simulator instance
   is destroyed.

   **Status**: Consent Reached in Discussion

 * Naming of applyUpdates methods on RfbStructure and LowLevelCar.

   In discussion: Simplify name of LowLevelCar::applyUpdates() to
   LowLevelCar::update(). Extend on the meaning of
   RfbStructure::applyUpdates() by renaming it to
   RfbStructure::updateCarsAndRestoreConsistency().

   **Status**: Consent Reached in Discussion

 * Naming of getNextCar() / getPrevCar() on RfbStructure.

   In discussion: The names are unclear. Rename the methods to
   getNextCarInFront() and getNextCarBehind().

   **Status**: Consent Reached in Discussion

 * RfbStructure::incorporateInsertedCars(): Is applyUpdates called?

   Does this method call applyUpdates() on the cars? This would result in a a
   more consistent API, all writers only need to update the next* fields of
   cars. There seems to be no particular advantages for either alternative.

   In discussion: Vehicle::applyUpdates() (now called Vehicle::update()) is
   called by RfbStructure::incorporateInsertedCars().

   **Status**: Consent Reached in Discussion

 * Cache Line Optimisations

   Do cache line optimisations on contiguous data arrays provide significant
   performance gains?

   On what structure should optimisations be performed? For example
   LowLevelCar will be wrapped in further structures by different RfbStructure
   implementations. Alignment is important as well.

   Optimisations can be applied manually by adding padding fields or
   automatically by some compilers by specifying additional keywords.

   **Status**: Pending

 * Inline short methods?

   Short methods should be inline-defined to enable advanced optimisations by
   the compiler. Especially getters will be optimised and will not result in a
   function call.

   In discussion: All small functions called very frequently during
   computation should be declared (including their body) in the header file.
   This is the case anyway mostly as large parts of the lowlevel module are
   defined as templates. Having access to the body allows the compiler to
   apply additional optimisations and expand the function at call location
   instead of performing a proper call.

   **Status**: Consent Reached in Discussion

 * Resolve template dependencies related to Simulator and computation routines

   Solution: All domain model data as well as the low level data structures
   used during computation are stored (embedded or as references) in a
   separate class instance, SimulationData. This class is embedded by
   Simulator and is referred to by computation routines. Defining
   SimulationData in a separate header resolves the circular dependency
   problems of the template definition.

   **Status**: Implemented
