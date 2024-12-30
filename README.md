# dmpath

Copyright (c) 2013-2018 brinkqiang (brink.qiang@gmail.com)

[dmpath GitHub](https://github.com/brinkqiang/dmpath)

## Build status
| [Linux][lin-link] | [Mac][mac-link] | [Windows][win-link] |
| :---------------: | :----------------: | :-----------------: |
| ![lin-badge]      | ![mac-badge]       | ![win-badge]        |

[lin-badge]: https://github.com/brinkqiang/dmpath/workflows/linux/badge.svg "linux build status"
[lin-link]:  https://github.com/brinkqiang/dmpath/actions/workflows/linux.yml "linux build status"
[mac-badge]: https://github.com/brinkqiang/dmpath/workflows/mac/badge.svg "mac build status"
[mac-link]:  https://github.com/brinkqiang/dmpath/actions/workflows/mac.yml "mac build status"
[win-badge]: https://github.com/brinkqiang/dmpath/workflows/win/badge.svg "win build status"
[win-link]:  https://github.com/brinkqiang/dmpath/actions/workflows/win.yml "win build status"


## Intro
```
dmpath --addpath=<path> or --removepath=<path>
```

## set pwd to path

unix like
```
dmpath --addpath=$(pwd) or --removepath=$(pwd)
```

win
```
dmpath --addpath="%cd%" or --removepath="%cd%"
```

## Thanks
