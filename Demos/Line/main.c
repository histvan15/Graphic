SDL_Event event;
while (SDL_WaitEvent(&event)) {
    if (event.type == SDL_QUIT) break;

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            printf("Kattintás helye: %d, %d\n", event.button.x, event.button.y);

            if (!is_drawing) {
                start_x = event.button.x;
                start_y = event.button.y;
                is_drawing = true;
            } else {
                if (line_count < MAX_LINE_COUNT) {
                    lines[line_count].x1 = start_x;
                    lines[line_count].y1 = start_y;
                    lines[line_count].x2 = event.button.x;
                    lines[line_count].y2 = event.button.y;
                    lines[line_count].color = current_color;
                    line_count++;
                }
                is_drawing = false;
            }
        }
    }
}
SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
SDL_RenderClear(renderer);

for (int i = 0; i < line_count; i++) {
    SDL_SetRenderDrawColor(renderer, lines[i].color.r, lines[i].color.g, lines[i].color.b, 255);
    SDL_RenderDrawLine(renderer, lines[i].x1, lines[i].y1, lines[i].x2, lines[i].y2);
}

if (is_drawing) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
    SDL_RenderDrawLine(renderer, start_x, start_y, mx, my);
}

SDL_RenderPresent(renderer);