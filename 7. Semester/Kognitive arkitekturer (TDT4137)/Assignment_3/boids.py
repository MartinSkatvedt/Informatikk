import math
from random import randint, uniform

import pygame as pg

# Color constants
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)

# Window Parameters
SCREEN_WIDTH = 800
SCREEN_HEIGHT = 600

# Parameters
NUM_BOIDS = 350
BOID_SIZE = 2
MAX_FORCE = 0.3
BOID_FRICTION = 0.75

MAX_SPEED = 6
MIN_SPEED = 3


SEPARATION = 0.05
SEPARATION_RADIUS = 15

ALIGNMENT = 0.1
ALIGNMENT_RADIUS = 40

COHESION = 0.0005
COHESION_RADIUS = 80

TURN_FACTOR = 0.3
MARGIN = 150


class Simulation:

    def __init__(self):
        pg.init()
        self.running = False
        self.clock = pg.time.Clock()
        self.screen = pg.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        self.screen_rect = self.screen.get_rect()
        self.fps = 60

        self.boids = []
        for i in range(NUM_BOIDS):
            self.boids.append(Boid(self, (randint(0, SCREEN_WIDTH), randint(0, SCREEN_HEIGHT))))

    def events(self):
        for event in pg.event.get():
            if event.type == pg.QUIT:
                self.running = False

    def draw(self):
        # Empty the last screen
        self.screen.fill(BLACK)

        # Draw all boids
        for boid in self.boids:
            boid.draw(self.screen)

        # Update the screen
        pg.display.update()

    def update(self):
        """
        Method for going one step in the simulation
        """
        for boid in self.boids:
            boid.update(self.boids)

    def run(self):
        """
        Runs the simulation
        """
        self.running = True
        #counter = 0
        while self.running:
            self.clock.tick(self.fps)
            self.events()
            self.update()
            self.draw()
            ##counter += 1
            #pg.image.save(self.screen, "images/image" + str(counter) + ".png")
            #if counter == 300:
                #self.running = False


class Boid():

    def __init__(self, simulation, position):
        self.simulation = simulation
        self.pos = pg.math.Vector2(position)

        self.acc = pg.math.Vector2(0, 0)
        self.heading = pg.math.Vector2(0, 0)

        self.vel = pg.math.Vector2(randint(-2, 2), randint(-2, 2))  # Random initial velocity
        self.friction = BOID_FRICTION  # Friction coefficient for the simplistic physics

        # Parameters for neighbors
        self.distance = 100
        self.angle = math.pi 

        self.neighbors = []

    def update(self, neighbors):
        """
        Updates the acceleration of the boid by adding together the different forces that acts on it
        """

        self.neighbors = []
        for neighbor in neighbors:
            if neighbor == self:
                continue
            # Calculate the distance to the neighbor
            d_to_boid = (neighbor.pos - self.pos).length()

            # If the neighbor is within the distance
            if d_to_boid < self.distance:
                angle = math.acos(self.heading.dot(neighbor.pos - self.pos) / d_to_boid)
                if angle <= self.angle:
                    self.neighbors.append(neighbor) 

 
        if len(self.neighbors) > 0:
            self.separation()
            self.alignment()
            self.cohesion() 


        if self.pos.x > self.simulation.screen_rect.w - MARGIN:
            self.vel.x -= TURN_FACTOR
        if self.pos.x < MARGIN:
            self.vel.x += TURN_FACTOR

        if self.pos.y > self.simulation.screen_rect.h - MARGIN:
            self.vel.y -= TURN_FACTOR
        if self.pos.y < MARGIN:
            self.vel.y += TURN_FACTOR

        if self.vel.length() < 0.001:
            self.vel = pg.math.Vector2(uniform(-1, 1), uniform(-1, 1))
        speed = self.vel.length()
        if speed > MAX_SPEED:
            self.vel = self.vel.normalize() * MAX_SPEED
        elif speed < MIN_SPEED:
            self.vel = self.vel.normalize() * MIN_SPEED
            
        self.pos += self.vel 

        # Simplistic surface friction
        self.vel *= self.friction

        # Calculate the heading
        angle = math.atan2(self.vel.y, self.vel.x)
        self.heading = pg.math.Vector2(math.cos(angle), math.sin(angle))

    def separation(self):
        """
        Calculate the separation force vector
        Separation: steer to avoid crowding local flockmates
        """
        close = pg.math.Vector2(0, 0) 
        
        for neighbor in self.neighbors:
            if (neighbor.pos - self.pos).length() < SEPARATION_RADIUS:
                close += self.pos - neighbor.pos 

        self.vel += close * SEPARATION

    def alignment(self):
        """
        Calculate the alignment force vector
        Alignment: steer towards the average heading of local flockmates
        """

        vel_avg = pg.math.Vector2(0, 0)
        count = 0
        for neighbor in self.neighbors:
            if (neighbor.pos - self.pos).length() < ALIGNMENT_RADIUS:
                vel_avg += neighbor.vel
                count+=1
        
        if count == 0:
            return pg.math.Vector2(0, 0)

        vel_avg /= count
        self.vel += (vel_avg - self.vel) * ALIGNMENT

    def cohesion(self):
        """
        Calculate the cohesion force vector
        Cohesion: steer to move toward the average position of local flockmates
        """

        pos_avg = pg.math.Vector2(0, 0)
        n_in_range = 0
        for neighbor in self.neighbors:
            if (neighbor.pos - self.pos).length() < COHESION_RADIUS:
                pos_avg += neighbor.pos
                n_in_range += 1

        if n_in_range == 0:
            return pg.math.Vector2(0, 0) 

        pos_avg /= n_in_range

        self.vel += (pos_avg - self.pos) * COHESION

    def draw(self, screen):
        """Draw boid to screen"""

        # Calculate the angle to the velocity vector to get the forward direction
        angle = math.atan2(self.vel.y, self.vel.x)
        other_points_angle = 0.75 * math.pi  # angle +- value to get the other two points in the triangle

        # Get the points of the triangle
        x0 = self.pos.x + BOID_SIZE * math.cos(angle)
        y0 = self.pos.y + BOID_SIZE * math.sin(angle)

        x1 = self.pos.x + BOID_SIZE * math.cos(angle + other_points_angle)
        y1 = self.pos.y + BOID_SIZE * math.sin(angle + other_points_angle)

        x2 = self.pos.x + BOID_SIZE * math.cos(angle - other_points_angle)
        y2 = self.pos.y + BOID_SIZE * math.sin(angle - other_points_angle)

        # Draw
        pg.draw.polygon(screen, WHITE, [(x1, y1), (x2, y2), (x0, y0)])

# Helper functions
def remap(n, start1, stop1, start2, stop2):
    """Remap a value in one range to a different range"""
    new_value = (n - start1) / (stop1 - start1) * (stop2 - start2) + start2
    if start2 < stop2:
        return constrain(new_value, start2, stop2)
    else:
        return constrain(new_value, stop2, start2)


def constrain(n, low, high):
    """Constrain a value to a range"""
    return max(min(n, high), low)


def limit(vector, length):
    """Cap a value"""
    if vector.length_squared() <= length * length:
        return
    else:
        vector.scale_to_length(length)


if __name__ == '__main__':
    sim = Simulation()
    sim.run()
