# xdrawrect
A utility program used to create X windows in the form of lines and rectangles.

## Dependencies
You will need to have the following libraries installed:

- libX11
- libXrandr

## Installation
To install the program, simply execute this in the root folder of the project:

~~~ sh
$ make install
~~~

## Usage
~~~ sh
$ xdrawrect <monitor> [top|bottom] width[%] height[%] x[%] y[%] color name [place_above_window_name]
~~~

The color should be specified in the following form: `#AARRGGBB`

The optional argument `place_above_window_name` will put the X window above and in the same
stack as the target. It needs to be the `WM_NAME` of an exising X window.

## Contributing
1. Fork the project
2. Create a feature branch: `git checkout -b my-new-feature`
3. Commit your changes: `git commit -am "Add some feature"`
4. Rebase against master: `git pull --rebase origin master`
5. Push to the branch: `git push origin my-new-feature`
6. Submit a pull request

## License
The project is licensed under the MIT license.
