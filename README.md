Hello!


# Overview

## Task 1

We have a TV visual effect that takes an input image and replaces all odd lines with a solid black color, in order to create a old-school interlaced television effect. This is the prototype. Please write an implementation for the function.  

```
void tv_effect( uint8_t const * const inBuffer, 
                const uint32_t width, 
                const uint32_t height,
                uint8_t* outBuffer  )
{
}
```

inBuffer - the image to work with as an array of pixels.
outBuffer - the returned, interlaced image as an array of pixels.
width - the number of pixels wide of the incoming image
height - the numbe of pixels tall of the incoming image

### Examples

In the main directory, you'll find two examples of image files that have been manipulated by this effect in Task 1.  You can use them to check your output to verify it matches.

### Language

You can choose to implement Task 1 in whatever language you prefer, as long as it's not something esoteric that none of us may know well.  Discuss with your interviewer if you like.


## Task 2

Please add comments to the NOTES.md file that explain what you did in your code.  While the code is a key part of this test, we're also interested in learning how you approached the task.  Some examples of questions you might address follow.
    
    * What kinds of issues did you encounter, if any?
    * How did you address those issues?
    * Would you do anything differently if you were worried about performance?
    * Would you do anything differently if you were worried about memory?
    * What else would be helpful to know about the problem?



# Logistics

1.  Please clone this repository.
2.  Create and checkout a branch that contains your name.
3.  Build your solution in that branch locally.
4.  Add any notes you'd like to include in the NOTES.md file.
5.  Commit your work to the branch, and push the branch to github.
6.  Issue a pull request for your branch to signal your submission.


# Time

The time alloted for completion will be discussed during the interview.
