
import json
import os
import random
import argparse
import enum
import sys
import math
import bisect
import functools
import itertools


try:
    choices = random.choices
except AttributeError:
    def choices(population, k=1):
        return list(map(
            random.choice,
            itertools.repeat(population, k),
        ))


class CardinalDirection(enum.IntEnum):
    NORTH = 0
    EAST = 1
    SOUTH = 2
    WEST = 3


class TurnDirection(enum.IntEnum):
    UTURN = 0
    LEFT = 1
    STRAIGHT = 2
    RIGHT = 3


class DrivingDirection(enum.IntEnum):
    DIR1 = 1
    DIR2 = 2


def direction_from_coordinates(origin, other):
    xOffset = other[0] - origin[0]
    yOffset = other[1] - origin[1]

    # Left-handed cartesian coordinate system, let's transform into
    # right-handed

    yOffset = -yOffset;

    if xOffset >= 0:
        if abs(yOffset) < xOffset:
            return CardinalDirection.EAST;
        elif yOffset >= 0:
            return CardinalDirection.NORTH;
        else:
            return CardinalDirection.SOUTH;
    else:
        if abs(yOffset) < -xOffset:
            return CardinalDirection.WEST;
        elif yOffset >= 0:
            return CardinalDirection.NORTH;
        else:
            return CardinalDirection.SOUTH;


class Junction(object):
    """High-level junction class used for output and exploring the model.

    """

    class Signal(object):
        def __init__(self, direction, duration):
            self.direction = direction
            self.duration = duration

        def export_json(self):
            return {
                "dir": self.direction,
                "time": self.duration,
            }

    class Connection(object):
        def __init__(self, road, junction, direction):
            self.road = road
            self.junction = junction
            self.direction = direction

    def __init__(self, x, y, id=None, signals=None, connections=None):
        self.x = x
        self.y = y
        self.id = id
        self.signals = signals or []
        self.connections = connections or []

    def connect(self, road, other_junction):
        connection = Junction.Connection(
            road,
            other_junction,
            direction_from_coordinates(
                (self.x, self.y),
                (other_junction.x, other_junction.y),
            ),
        )
        self.connections.append(connection)

    def export_json(self):
        return {
            "id": self.id,
            "x": self.x,
            "y": self.y,
            "signals": list(map(lambda x: x.export_json(), self.signals)),
        }

    @classmethod
    def from_coordinates(cls, x, y, id=None):
        return cls(x, y, id=id)


class Road(object):
    """High-level road class used for output and exploring the model.

    """

    def __init__(self, junction1, junction2, lanes=None, speedLimit=None):
        self.junction1 = junction1
        self.junction2 = junction2
        self.lanes = lanes
        self.speedLimit = speedLimit

    def length(self):
        return 100.0 * math.hypot(
            self.junction1.x - self.junction2.x,
            self.junction1.y - self.junction2.y,
        )


    def export_json(self):
        return {
            "junction1": self.junction1.id,
            "junction2": self.junction2.id,
            "lanes": self.lanes,
            "limit": self.speedLimit,
        }

    @classmethod
    def from_junctions(cls, junction1, junction2):
        return cls(junction1, junction2)


class Car(object):
    """High-level car class used for output and exploring the model.

    """

    class Position(object):
        def __init__(self, road, direction, lane, distance):
            self.road = road
            self.direction = direction
            self.lane = lane
            self.distance = distance

        def export_json(self):
            if self.direction == DrivingDirection.DIR1:
                from_junction = self.road.junction1
                to_junction = self.road.junction2
            elif self.direction == DrivingDirection.DIR2:
                from_junction = self.road.junction2
                to_junction = self.road.junction1

            return {
                "from": from_junction.id,
                "to": to_junction.id,
                "lane": self.lane,
                "distance": self.distance,
            }

    def __init__(
        self,
        id = None,
        target_velocity = None,
        max_acceleration = None,
        target_deceleration = None,
        min_distance = None,
        target_headway = None,
        politeness = None,
        route = None,
    ):
        self.id = id
        self.target_velocity = target_velocity
        self.max_acceleration = max_acceleration
        self.target_deceleration = target_deceleration
        self.min_distance = min_distance
        self.target_headway = target_headway
        self.politeness = politeness
        self.route = route

    def export_json(self):
        return {
            "id": self.id,
            "target_velocity": self.target_velocity,
            "max_acceleration": self.max_acceleration,
            "target_deceleration": self.target_deceleration,
            "min_distance": self.min_distance,
            "target_headway": self.target_headway,
            "politeness": self.politeness,
            "start": self.position.export_json(),
            "route": self.route,
        }


class Implementable(object):
    IMPLEMENTATIONS = None

    def __index__(self, *args, **kwargs):
        super(Implementable, self).__init__(*args, **kwargs)

    @classmethod
    def implementation_from_dict(cls, kind, dct):
        return cls.IMPLEMENTATIONS[kind].from_dict(dct)

    @classmethod
    def implementation_from_spec_dict(cls, dct):
        return cls.implementation_from_dict(dct["kind"], dct["spec"])


class Gridder(Implementable):
    IMPLEMENTATIONS = dict()

    def __init__(self, width=None, height=None):
        """Gridder constructor.

        Args:
            width (int, optional): Width of the grid. When set, the grids
                maximum x coordinate is width - 1.
            height (int, optional): Height of the grid. When set, the grids
                maximum y coordinate is height - 1.
        """

        self.width = width
        self.height = height

        self.reset()

    def reset(self):
        #: list(tuple): Internal junctions, stored as tuple(x, y)
        self._junctions = []
        #: list(set): Internal roads, stored as set(tuple(x1, y1), tuple(x2, y2))
        self._roads = []

        #: list(obj): Exposed junctions, using an appropriate class
        self.junctions = []
        #: list(obj): Exposed roads, using an appropriate class
        self.roads = []

    def __call__(self):
        self._generate()
        self._build_exposed_lists()
        return (self.junctions, self.roads)

    def _build_exposed_lists(self, junction_cls=Junction, road_cls=Road):
        self.junctions = list(
            junction_cls.from_coordinates(x, y, id=i)
            for (i, (x, y)) in enumerate(self._junctions),
        )

        ind = self._junctions.index
        lookup = lambda coordinate_pair: self.junctions[ind(coordinate_pair)]

        self.roads = list(
            road_cls.from_junctions(
                lookup(a),
                lookup(b),
            ) for (a, b) in self._roads,
        )

        for road in self.roads:
            road.junction1.connect(road, road.junction2)
            road.junction2.connect(road, road.junction1)

    def _generate(self):
        raise NotImplementedError

    def _connect(self, a, b):
        """Connects the two junctions a and b.

        This function is meant to be called by child classes only.

        Args:
            a (tuple(int, int)): Coordinates of junction a.
            b (tuple(int, int)): Coordinates of junction b.
        """
        if {a, b} not in self._roads:
            self._roads.append({a, b})


class CompleteGridder(Gridder):
    def __init__(self, width, height, spacing=1):
        super(CompleteGridder, self).__init__(width=width, height=height)
        self.spacing = spacing

    def _generate(self):
        x_gen = range(0, self.width, self.spacing)
        y_gen = range(0, self.height, self.spacing)

        self._junctions.extend(
            (x, y) for x in x_gen for y in y_gen,
        )

        for (x, y) in self._junctions:
            x_other = x + self.spacing
            y_other = y + self.spacing

            if x_other < self.width:
                self._connect((x, y), (x_other, y))

            if y_other < self.height:
                self._connect((x, y), (x, y_other))

    @classmethod
    def from_dict(cls, dct):
        return cls(
            dct["width"],
            dct["height"],
            spacing = dct.get("spacing", 1),
        )

Gridder.IMPLEMENTATIONS["complete"] = CompleteGridder


class Signaliser(Implementable):
    IMPLEMENTATIONS = dict()

    def __call__(self, junctions):
        raise NotImplementedError


class GaussSignaliser(Signaliser):
    def __init__(self, mu, sigma, min=None, max=None):
        super(GaussSignaliser, self).__init__()
        self._dist = DistributionCutter(
            random.gauss, mu, sigma, min=min, max=max
        )

    def __call__(self, junctions):
        for junction in junctions:
            directions = list(
                set(conn.direction for conn in junction.connections),
            )
            random.shuffle(directions)

            def signal_durations():
                while True:
                    yield int(self._dist())

            junction.signals = list(
                Junction.Signal(dir, int(duration))
                for (dir, duration) in zip(directions, signal_durations())
            )

    @classmethod
    def from_dict(cls, dct):
        return cls(
            dct["mu"],
            dct["sigma"],
            min = dct.get("min"),
            max = dct.get("max"),
        )

Signaliser.IMPLEMENTATIONS["gauss"] = GaussSignaliser


class RoadPopulater(Implementable):
    IMPLEMENTATIONS = dict()

    def __call__(self, roads):
        for road in roads:
            self._populate_road(road)

    def _populate_road(self, road):
        raise NotImplementedError


class RandomRoadPopulater(RoadPopulater):
    def __init__(self, lanes_dist, limit_dist):
        super(RandomRoadPopulater, self).__init__()
        self._lanes_dist = lanes_dist
        self._limit_dist = limit_dist

    def _populate_road(self, road):
        road.lanes = int(self._lanes_dist())
        road.speedLimit = self._limit_dist()

    @classmethod
    def from_dict(cls, dct):
        return cls(
            distribution_from_spec_dict(dct["lanes"]),
            distribution_from_spec_dict(dct["limit"]),
        )

RoadPopulater.IMPLEMENTATIONS["random"] = RandomRoadPopulater


class CarGenerator(Implementable):
    IMPLEMENTATIONS = dict()

    def __call__(self):
        raise NotImplementedError


class RandomCarGenerator(CarGenerator):
    def __init__(self,
        number_dist,
        target_velocity_dist,
        max_acceleration_dist,
        target_deceleration_dist,
        min_distance_dist,
        target_headway_dist,
        politeness_dist,
        route_length_dist,
    ):
        super(RandomCarGenerator, self).__init__()
        self._number_dist = number_dist
        self._target_velocity_dist = target_velocity_dist
        self._max_acceleration_dist = max_acceleration_dist
        self._target_deceleration_dist = target_deceleration_dist
        self._min_distance_dist = min_distance_dist
        self._target_headway_dist = target_headway_dist
        self._politeness_dist = politeness_dist
        self._route_length_dist = route_length_dist

        self.reset()

    def reset(self):
        self.cars = []

    def __call__(self):
        self.cars = list(
            map(self._generate_car, range(self._number_dist())),
        )

        return self.cars

    def _generate_car(self, id):
        return Car(
            id = id,
            target_velocity = self._target_velocity_dist(),
            max_acceleration = self._max_acceleration_dist(),
            target_deceleration = self._target_deceleration_dist(),
            min_distance = self._min_distance_dist(),
            target_headway = self._target_headway_dist(),
            politeness = self._politeness_dist(),
            route = self._generate_route(),
        )

    def _generate_route(self):
        route_length = self._route_length_dist()

        turn_directions = (
            TurnDirection.UTURN,
            TurnDirection.LEFT,
            TurnDirection.STRAIGHT,
            TurnDirection.RIGHT,
        )

        return choices(turn_directions, k=route_length)

    @classmethod
    def from_dict(cls, dct):
        return cls(
            distribution_from_spec_dict(dct["number"]),
            distribution_from_spec_dict(dct["target_velocity"]),
            distribution_from_spec_dict(dct["max_acceleration"]),
            distribution_from_spec_dict(dct["target_deceleration"]),
            distribution_from_spec_dict(dct["min_distance"]),
            distribution_from_spec_dict(dct["target_headway"]),
            distribution_from_spec_dict(dct["politeness"]),
            distribution_from_spec_dict(dct["route_length"]),
        )

CarGenerator.IMPLEMENTATIONS["random"] = RandomCarGenerator

class Positioner(Implementable):
    IMPLEMENTATIONS = dict()

    def __call__(self, cars, junctions, roads):
        raise NotImplementedError


class RandomPositioner(Positioner):
    class DensityModel(enum.Enum):
        ROAD_COUNT = "road_count"
        LANE_COUNT = "lane_count"
        ROAD_LENGTH = "road_length"
        LANE_LENGTH = "lane_length"

    _density_map_func = {
        DensityModel.ROAD_COUNT:
            lambda road: 1,
        DensityModel.LANE_COUNT:
            lambda road: road.lanes,
        DensityModel.ROAD_LENGTH:
            lambda road: road.length(),
        DensityModel.LANE_LENGTH:
            lambda road: road.lanes * road.length(),
    }

    def __init__(self, density_model):
        super(Positioner, self).__init__()
        self._density_model = density_model

    def __call__(self, cars, junctions, roads):
        cum_density, cum_density_road = zip(
            *self._build_cumulative_density_function(roads),
        )

        for car in cars:
            rnd = random.uniform(0.0, cum_density[-1])

            ind = bisect.bisect_right(cum_density, rnd)
            road = cum_density_road[ind]

            base = cum_density[ind - 1] if ind > 0 else 0
            # on_road_rnd el [0, 2.0 * road.lanes)
            on_road_rnd = (
                # Either direction on the road
                2.0 *
                # Any lane
                float(road.lanes) *
                # Project remaining rnd onto road
                (rnd - base) / (cum_density[ind] - base)
            )

            direction = math.floor(on_road_rnd) // road.lanes
            lane = math.floor(on_road_rnd) % road.lanes

            on_road_rnd -= math.floor(on_road_rnd)
            distance = road.length() * on_road_rnd

            car.position = Car.Position(
                road,
                DrivingDirection(direction + 1),
                lane,
                distance,
            )

    def _build_cumulative_density_function(self, roads, density_model=None):
        return zip(
            itertools.accumulate(
                map(
                    self._density_map_func[self._density_model],
                    roads,
                ),
            ),
            roads,
        )

    @classmethod
    def from_dict(cls, dct):
        return cls(
            cls.DensityModel(dct["density_model"]),
        )

Positioner.IMPLEMENTATIONS["random"] = RandomPositioner


class TimeStepper(Implementable):
    IMPLEMENTATIONS = dict()

    def __call__(self):
        self.time_steps = self._draw_time_steps()
        return self.time_steps

    def _draw_time_steps(self):
        raise NotImplementedError


class RandomTimeStepper(TimeStepper):
    def __init__(self, dist):
        super(RandomTimeStepper, self).__init__()
        self._dist = dist

    def _draw_time_steps(self):
        return self._dist()

    @classmethod
    def from_dict(cls, dct):
        return cls(
            distribution_from_spec_dict(dct)
        )

TimeStepper.IMPLEMENTATIONS["random"] = RandomTimeStepper


class Generator(object):
    def __init__(
        self,
        time_stepper,
        gridder,
        signaliser,
        road_populater,
        car_generator,
        positioner,
    ):
        self._time_stepper = time_stepper
        self._gridder = gridder
        self._signaliser = signaliser
        self._road_populater = road_populater
        self._car_generator = car_generator
        self._positioner = positioner

        self.reset()

    def reset(self):
        self.roads = []
        self.junctions = []
        self.cars = []
        self.time_steps = None

        sub_generators = (
            self._gridder,
            self._signaliser,
            self._road_populater,
            self._car_generator,
            self._positioner,
        )

        for generator in sub_generators:
            try:
                generator.reset()
            except AttributeError:
                pass


    def __call__(self):
        self.time_steps = self._time_stepper()

        self.junctions, self.roads = self._gridder()
        self._signaliser(self.junctions)
        self._road_populater(self.roads)

        self.cars = self._car_generator()

        self._positioner(self.cars, self.junctions, self.roads)

    def export_json(self):
        return {
            "time_steps": self.time_steps,
            "junctions": list(map(
                lambda junction: junction.export_json(),
                self.junctions,
            )),
            "roads": list(map(
                lambda road: road.export_json(),
                self.roads,
            )),
            "cars": list(map(
                lambda car: car.export_json(),
                self.cars,
            )),
        }

    @classmethod
    def from_dict(cls, dct):
        return cls(
            TimeStepper.implementation_from_spec_dict(dct["time_step"]),
            Gridder.implementation_from_spec_dict(dct["grid"]),
            Signaliser.implementation_from_spec_dict(dct["signal"]),
            RoadPopulater.implementation_from_spec_dict(dct["road"]),
            CarGenerator.implementation_from_spec_dict(dct["car"]),
            Positioner.implementation_from_spec_dict(dct["position"]),
        )


class DistributionCutter(object):
    def __init__(
        self,
        distribution,
        *args,
        min = None,
        max = None,
        type = float,
        **kwargs
    ):
        super(DistributionCutter, self).__init__()

        self._distribution = functools.partial(distribution, *args, **kwargs)

        def cut(x):
            if min is not None and x < min:
                return min

            if max is not None and x > max:
                return max

            return x

        def ensure_type(x):
            if not isinstance(x, type):
                return type(x)
            else:
                return x

        self._cut = cut
        self._ensure_type = ensure_type

    def __call__(self):
        return self._ensure_type(
            self._cut(
                self._distribution(),
            ),
        )


def random_gauss_int(mu, sigma):
    return round(random.gauss(mu, sigma))

def distribution_from_dict(kind, dct):
    if kind == "gauss":
        return DistributionCutter(
            random.gauss,
            dct["mu"],
            dct["sigma"],
            min = dct.get("min"),
            max = dct.get("max"),
        )
    elif kind == "uniform":
        return functools.partial(random.uniform, dct["min"], dct["max"])
    elif kind == "gaussint":
        return DistributionCutter(
            random_gauss_int,
            dct["mu"],
            dct["sigma"],
            min = dct.get("min"),
            max = dct.get("max"),
            type = int,
        )
    elif kind == "uniformint":
        return functools.partial(
            random.randrange,
            dct["min"],
            dct["max"],
            dct.get("step", 1),
        )
    elif kind == "static":
        value = dct["value"]
        return lambda: value

def distribution_from_spec_dict(dct):
    return distribution_from_dict(dct["kind"], dct["spec"])


parser = argparse.ArgumentParser(
    description = 'Generate simulation input files',
)
parser.add_argument(
    'config_path',
    metavar = 'config-path',
    type = str,
    help = 'Path to the generator configuration to be used. '
           'If the special value "-" is passed, the configuration is read '
           'from stdin.',
)
parser.add_argument(
    '--number', '-n',
    type = int,
    default = 1,
    help = 'The number of files to be generated. (default 1)',
)
parser.add_argument(
    '--seed',
    type = str,
    help = 'Seed to be used for the pseudo-random number generator. '
           'The value is expected in hex representation. '
           'If omitted, a seed is generated from a suitable source.',
)
parser.add_argument(
    '--output-path',
    default = '-',
    help = 'Write generated output to files instead of to stdout. '
           'A total of the passed --number files. '
           'If the special value "-" is passed, the output is written to '
           'stdout (this is the default). '
           'Variables available for substitution: '
           '{seed} the seed for the current file in hex representation, '
           '{n} the number of files to be generated,'
           '{i} the index of the current file (starting at 0).'
)


def reseed(seed=None, n=16):
    if seed is None:
        seed = os.urandom(n)

    random.seed(seed)
    return seed


def main():
    args = parser.parse_args()

    _validate_args(args)

    if args.config_path == "-":
        config_dict = json.load(sys.stdin)
    else:
        with open(args.config_path) as f:
            config_dict = json.load(f)

    if args.number == 1:
        _main_single(args, config_dict)
    else:
        _main_many(args, config_dict)

def _validate_args(args):
    if args.number <= 0:
        print(
            "The number of files to be generated (--number) needs to be "
            "greater than 0.",
            file = sys.stderr,
        )
        sys.exit(1)

    if args.output_path is None and args.number > 1:
        print(
            "Cannot output to stdout when the number of files to be "
            "generated is greater than 1."
            "\n"
            "Use the --output-path argument instead.",
            file = sys.stderr,
        )
        sys.exit(1)

    if args.output_path is not None and args.number > 1 \
        and "{seed}" not in args.output_path \
        and "{i}" not in args.output_path:
        print(
            "The output path must contain variables when the number of files "
            "to be generated is greater than 1.",
            file = sys.stderr,
        )
        sys.exit(1)

    if args.seed is not None and args.number > 1:
        print(
            "Cannot use --seed argument when number of files to be generated "
            "is greater than 1.",
            file = sys.stderr,
        )
        sys.exit(1)

def _main_single(args, config_dict):
    if args.seed is None:
        seed = reseed()
        print(
            "Generated new seed; "
            "hex representation: {seed}".format(seed=seed.hex()),
            file=sys.stderr,
        )
    else:
        seed = reseed(bytes.fromhex(args.seed))
        print(
            "Using seed passed as argument; "
            "hex representation: {seed}".format(seed=seed.hex()),
            file=sys.stderr,
        )

    generator = Generator.from_dict(config_dict)

    generator()

    _write_simulation_file(
        args.output_path,
        generator.export_json(),
        seed,
    )

def _main_many(args, config_dict):
    generator = Generator.from_dict(config_dict)

    for i in range(args.number):
        seed = reseed()
        generator.reset()

        generator()

        _write_simulation_file(
            args.output_path,
            generator.export_json(),
            seed,
            i = i,
            n = args.number,
        )

def _write_simulation_file(output_path, obj, seed, i=0, n=1):
    if output_path == "-":
        json.dump(obj, sys.stdout)
    else:
        path = output_path.format(seed=seed.hex(), i=i, n=n)
        with open(path, "w") as f:
            json.dump(obj, f)

if __name__ == '__main__':
    main()
