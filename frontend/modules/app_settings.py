"""App-wide UI settings — PyDracula style."""


class Settings:
    MENU_WIDTH = 240
    MENU_WIDTH_COLLAPSED = 60
    TIME_ANIMATION = 180

    MENU_SELECTED_STYLESHEET = """
    border-left: 22px solid qlineargradient(
        spread:pad, x1:0.034, y1:0, x2:0.216, y2:0,
        stop:0.499 rgba(189, 147, 249, 255),
        stop:0.5 rgba(85, 170, 255, 0)
    );
    background-color: rgb(40, 44, 52);
    """
