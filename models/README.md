# Example Models for Maze Builder

### How to customize line segments

The base of a line segment is made from the built-in Gazebo base `box` structure.

You will need to adjust the width and height here:
```
<box>
    <size>0.06 1 0.000937</size>
</box>
```

In the `size` tags, the first value represents the width, and the second value represents the height. You will need to adjust this value in the `visual` and `collision` tags.