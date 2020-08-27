<?php

/**
 * @file
 * MF Homepage editors picks block template.
 */
?>

<?php if (!empty($items)): ?>
  <?php foreach ($items as $item): ?>
    <a href="/<?php print $item['path']; ?>">
      <div class="editors-pick__item">
        <div class="media-block media-block media-block_tile">
          <?php if (!empty($item['image'])): ?>
            <div class="media-block__media">
              <?php if ($item['is_video']): ?>
                <svg class="icon svg-icon_play_80-dims">
                  <use xmlns:xlink="http://www.w3.org/1999/xlink" xlink:href="#icon_play_80"></use>
                </svg>
              <?php endif; ?>
              <?php if ($item['is_gallery']): ?>
                <svg class="icon svg-icon_gallery_80-dims">
                  <use xmlns:xlink="http://www.w3.org/1999/xlink" xlink:href="#icon_gallery_80"></use>
                </svg>
              <?php endif; ?>
              <?php if ($item['todays_news']): ?>
                <svg class="icon svg-icon_trending-dims">
                  <use xmlns:xlink="http://www.w3.org/1999/xlink" xlink:href="#icon_trending"></use>
                </svg>
              <?php endif; ?>
              <?php print $item['image']; ?>
            </div>
          <?php endif; ?>
          <div class="media-block__body">
            <?php if (!empty($item['title'])): ?>
              <strong class="media-block__headline">
                <span class="media-block__headline-text"><?php print $item['title']; ?></span>
              </strong>
            <?php endif; ?>

            <div class="media-block__meta">
              <?php if (!empty($item['topics'])): ?>
                <span class="category-link editors-pick__category-link <?php !empty($item['class']) ? print $item['class'] : ''; ?>">
                <?php print $item['topics']; ?>
              </span>
              <?php endif; ?>
              <?php if (!empty($item['author'])): ?>
                <span class="media-block__byline"><?php print $item['author']; ?></span>
              <?php endif; ?>
              <?php if (!empty($item['todays'])): ?>
                <span class="media-block__today">
                <svg class="icon svg-icon_time-dims">
                  <use xmlns:xlink="http://www.w3.org/1999/xlink" xlink:href="#icon_time"></use>
                </svg>
                Today
              </span>
              <?php endif; ?>
            </div>
          </div>
        </div>
      </div>
    </a>
  <?php endforeach; ?>
<?php endif; ?>
