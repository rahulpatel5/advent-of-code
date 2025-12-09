"""
FOR each pair of points:
    IF area is bigger than current biggest area:
        STORE as biggest area

RETURN biggest area
"""

def get_largest_area(input: list) -> int:
    """
    Takes coordinates on map and finds largest area using two points.

    Args:
        input (list): list of coordinates for points

    Returns:
        int: largest area of a rectangle using two points
    """
    
    points: list = []
    for row in input:
        points.append(list(map(int, row.split(','))))
    
    biggest_area: int = 0
    for point1_index, point1 in enumerate(points):
        for point2 in points[point1_index + 1:]:
            if (abs(point2[0] - point1[0]) + 1) * (abs(point2[1] - point1[1]) + 1) > biggest_area:
                biggest_area = (abs(point2[0] - point1[0]) + 1) * (abs(point2[1] - point1[1]) + 1)
    
    return biggest_area
