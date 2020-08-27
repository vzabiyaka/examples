<?php


/**
 * @file
 * MFBlocks class file.
 */

/**
 * Class to process and display MF blocks.
 *
 * Class MFBlocks.
 */
class MFBlocks {

  /**
   * Default image file id.
   */
  const DEFAULT_IMAGE_FILE_ID = 81456;

  /**
   * Helper to get promo title value.
   *
   * @param object|int|string $node
   *   Node object or node id.
   *
   * @return string
   *   Promo title field output.
   */
  protected static function getPromoTitle($node) {
    $promo_title = MFLoadEntities::getEntityFieldValue('node', $node, 'field_promo_title');
    if (empty($promo_title)) {
      $promo_title = MFLoadEntities::getEntityFieldValue('node', $node, 'title');
    }
    return truncate_utf8($promo_title, 70, TRUE);
  }

  /**
   * Helper to get promo title value.
   *
   * @param object|int|string $node
   *   Node object or node id.
   * @param string $image_style
   *   Image style to generate.
   * @param array $attributes
   *   Image style to generate.
   *
   * @return string
   *   Promo title field output.
   */
  public static function getPromoImage($node, $image_style, $attributes = array()) {
    $file = MFLoadEntities::getEntityFieldValue('node', $node, 'field_promo_image');
    if (empty($file)) {
      $file = MFLoadEntities::getEntityFieldValue('node', $node, 'field_images');
    }
    if (!$file && isset($attributes['default_fallback_image'])) {
      $file = file_load(self::DEFAULT_IMAGE_FILE_ID);
      if (!empty($file)) {
        $file = (array) $file;
      }
      unset($attributes['default_fallback_image']);
      
    }
    // Return image style.
    if (!empty($file)) {
      return self::imageStyleRender($file, $image_style, $attributes);
    }
  }

    /**
     * Helper to get Top3 nodes from same channel.
     *
     * @param null|object $node
     *   Node object.
     *
     * @return string
     *   Top 3 from channel block render array.
     */
    public static function getTop3FromChannel($node = NULL) {
        if (empty($node)) {
            $node = menu_get_object();
        }

        if (!empty($node)) {
            $general_category = MFLoadEntities::getEntityFieldValue('node', $node, 'field_primary_categories');
            $module_path = self::getPath('module', 'mf_homepage');
            if (!empty($general_category->tid)) {
                $tids = self::getTermChildrenTids($general_category->tid);
                $nodes = MFLoadEntities::preloadNids(3, NULL, $tids, NULL, array($node->nid));
                $items = node_view_multiple($nodes, 'teaser');
                $items = reset($items);
                unset($items['#sorted']);
                $data = array(
                    '#theme' => 'mf_node_channel_top3',
                    '#channel' => t('Top 3 in @channel', array('@channel' => $general_category->name)),
                    '#items' => $items,
                );

                $js_settings = array(
                    'nid' => $node->nid,
                    'tid' => $general_category->tid,
                );
                $data['#attached'] = array(
                    'js' => array(
                        array(
                            'data' => array('mfNode' => $js_settings),
                            'type' => 'setting',
                        ),
                    )
                );
                $data['#attached']['js'][$module_path . '/js/vendor/jquery.waypoints.min.js'] = array(
                    'scope' => 'footer',
                );
                $data['#attached']['js'][$module_path . '/js/vendor/jquery.visible.min.js'] = array(
                    'scope' => 'footer',
                );
                // Add load more scripts and settings to not sponsored nodes only.
                $sponsored = MFLoadEntities::getEntityFieldValue('node', $node, 'field_sponsored');
                $types = array('article', 'video', 'recipe', 'photo_gallery');
                if (in_array($node->type, $types)) {
                    if (empty($sponsored)) {
                        $data['#attached']['js'][$module_path . '/js/vendor/jquery.waypoints.min.js'] = array(
                            'scope' => 'footer',
                        );
                        $data['#attached']['js'][$module_path . '/js/vendor/infinite.js'] = array(
                            'scope' => 'footer',
                        );
                        $data['#attached']['js'][$module_path . '/js/mf_node_load-more.js'] = array(
                            'scope' => 'footer',
                        );
                        $data['#attached']['js'][$module_path . '/js/vendor/jquery.visible.min.js'] = array(
                            'scope' => 'footer',
                        );
                    }
                    else {
                        $data['#attached']['js'][$module_path . '/js/mf_node_common.js'] = array(
                            'scope' => 'footer',
                        );
                    }
                }

                return $data;
            }
        }
    }

    /**
     * Helper to prepare related articles output.
     *
     * @param object|int|string $node
     *   Node object ot id.
     *
     * @return array
     *   Rendered related articles items.
     */
    public static function prepareRelatedArticles($node) {
        $data = array();
        $related_items = MFLoadEntities::getEntityFieldValue('node', $node, 'field_related_articles');
        foreach ($related_items as $key => $related_item) {
            if ($related_item) {
                $content = array();
                $attributes = array('class' => array('block-media__img'));
                $content['image'] = self::getPromoImage($related_item, 'raight_rail_image', $attributes);
                $content['title'] = self::getPromoTitle($related_item);
                $content['subtitle'] = self::getPromoDescription($related_item);
                $class = self::getChannelClass($related_item);
                $content['topics'] = self::getTopics($related_item, array('attributes' => array('class' => array('category-link', $class))));
                $content['path'] = drupal_get_path_alias('node/' . $related_item->nid);
                $content['is_todays_news'] = MFLoadEntities::checkTodaysNews($related_item->nid);
                $data[$key] = theme('mf_node_related_article', $content);
            }
        }

        return $data;
    }

    /**
     * Helper to get Top3 nodes from same channel.
     */
    public static function getSponsoredByBlock($sponsored_by = NULL, $image_classes = NULL) {
        if (!$sponsored_by) {
            $node = menu_get_object();
            if ($node) {
                $sponsored_by = MFLoadEntities::getEntityFieldValue('node', $node, 'field_sponsored_by_logo');
            }
        }
        if ($sponsored_by) {
            // Prepare images info array.
            $images_info = array(
                'field_sponsored_image' => 'desktop',
                'field_mobile_sponsored_image' => 'mobile',
            );

            $link = MFLoadEntities::getEntityFieldValue('taxonomy_term', $sponsored_by, 'field_sponsored_image_url');
            $images = array();
            foreach ($images_info as $field => $image) {
                $file = MFLoadEntities::getEntityFieldValue('taxonomy_term', $sponsored_by, $field);
                if (!empty($file)) {
                    $classes = 'sponsored-img';
                    if ($image_classes) {
                        $classes .= ' ' . $image_classes;
                    }
                    $images[$image] = theme('image', array(
                        'path' => file_create_url($file['uri']),
                        'attributes' => array('class' => $classes),
                    ));
                }
            }

            return array(
                '#theme' => 'mf_node_sponsored_by_logo',
                '#images' => $images,
                '#link' => $link,
            );
        }
    }

    /**
     * Helper to output workout routine workout parts field.
     */
    public static function getWorkoutRoutineParts() {
        $node = menu_get_object();
        $module_path = self::getPath('module', 'mf_node');
        if (!empty($node) && $node->type == 'workout_routine') {
            $days = array();
            $workout_parts = MFLoadEntities::getEntityFieldValue('node', $node, 'field_parts');
            if (!empty($workout_parts)) {
                foreach ($workout_parts as $key => $workout_part) {
                    $days[$key]['number'] = MFLoadEntities::getEntityFieldValue('field_collection_item', $workout_part, 'field_part_name');
                    $days[$key]['title'] = MFLoadEntities::getEntityFieldValue('field_collection_item', $workout_part, 'field_part_subtitle');
                    $image = MFLoadEntities::getEntityFieldValue('field_collection_item', $workout_part, 'field_image');
                    if (!empty($image)) {
                        $days[$key]['image'] = self::imageStyleRender($image, 'wide_videos', array('class' => array('article-media__obj')));
                    }

                    $video_id = MFLoadEntities::getEntityFieldValue('field_collection_item', $workout_part, 'field_jw_player_video_id');
                    if (!empty($video_id)) {
                        $days[$key]['popup_path'] = 'workout-routine/video/' . $node->nid . '/' . $video_id;
                        $days[$key]['mobile_popup_path'] = 'workout-routine/video-mobile/' . $node->nid . '/' . $video_id;
                        // If no image but jw player video id populated use placeholder.
                        if (empty($image)) {
                            // Use default file.
                            $file = file_load(self::DEFAULT_IMAGE_FILE_ID);
                            if (!empty($file)) {
                                $file = (array) $file;
                                $days[$key]['image'] = self::imageStyleRender($file, 'wide_videos', array('class' => array('article-media__obj')));
                            }
                        }
                    }
                    $fc_part_items = MFLoadEntities::getEntityFieldValue('field_collection_item', $workout_part, 'field_part_item');
                    if (!empty($fc_part_items)) {
                        $days[$key]['steps'] = array();
                        foreach ($fc_part_items as $i => $fc_part_item) {
                            // Add exercise title ans path as theme variables.
                            $exercise_node = MFLoadEntities::getEntityFieldValue('field_collection_item', $fc_part_item, 'field_part_exercises');
                            if (!empty($exercise_node) && $exercise_node->status == 1) {
                                $days[$key]['steps'][$i]['title'] = $exercise_node->title;
                                $days[$key]['steps'][$i]['exercise_path'] = drupal_get_path_alias('node/' . $exercise_node->nid);

                                $days[$key]['steps'][$i]['number'] = MFLoadEntities::getEntityFieldValue('field_collection_item', $fc_part_item, 'field_number_override');
                                $days[$key]['steps'][$i]['sets'] = MFLoadEntities::getEntityFieldValue('field_collection_item', $fc_part_item, 'field_part_sets');
                                $days[$key]['steps'][$i]['reps'] = MFLoadEntities::getEntityFieldValue('field_collection_item', $fc_part_item, 'field_part_reps');
                                $days[$key]['steps'][$i]['rest'] = MFLoadEntities::getEntityFieldValue('field_collection_item', $fc_part_item, 'field_part_rest');

                                $editorial_details = MFLoadEntities::getEntityFieldValue('field_collection_item', $fc_part_item, 'field_part_editorial_details');
                                if (empty($editorial_details)) {
                                    $days[$key]['steps'][$i]['description'] = MFLoadEntities::getEntityFieldValue('node', $exercise_node, 'field_subtitle');
                                }
                                else {
                                    $days[$key]['steps'][$i]['description'] = $editorial_details;
                                }
                            }
                        }
                    }
                }
            }

            return array(
                '#theme' => 'mf_node_workout_routine_parts',
                '#days' => $days,
                '#attached' => array(
                    'js' => array(
                        $module_path . '/js/mf_node_video-overlay.js' => array(
                            'scope' => 'footer',
                        ),
                    ),
                ),
            );
        }
    }

    /**
     * Helper to get node topics color related class.
     *
     * @param object $node
     *   Node object.
     *
     * @return mixed|string
     *   Class to set color of the link.
     */
    public static function getChannelClass($node) {
        if (is_object($node)) {
            // Return grey color for product pages.
            if ($node->type == 'product') {
                return 'grey';
            }
            $general_category = MFLoadEntities::getEntityFieldValue('node', $node, 'field_primary_categories');
            if (!empty($general_category->tid)) {
                $colors = array(
                    // Nutrition.
                    299 => 'green',
                    // Women.
                    304 => 'cyan',
                    // Life.
                    308 => 'blue',
                    // Style & Grooming.
                    296 => 'blue',
                );
                if (MFNodeBlocks::checkTermDepth($general_category->tid) == 1) {
                    if (!empty($colors[$general_category->tid])) {
                        return $colors[$general_category->tid];
                    }
                }
                else {
                    $parents = taxonomy_get_parents($general_category->tid);
                    if (!empty($parents) && is_array($parents)) {
                        $parent = reset($parents);
                        if (!empty($parent->tid) && !empty($colors[$parent->tid])) {
                            return $colors[$parent->tid];
                        }
                    }
                }
            }
        }
    }

    /**
     * Helper to prepare mobile top image or video for amp ct's on non amp view.
     *
     * @param object|int|string $node
     *   Node object ot id.
     *
     * @param bool $video
     *   Load video insted of image
     *
     * @return array
     *   Rendered related articles items.
     */
    public static function prepareTopMobileImage($node, $node_type) {
        if (in_array($node_type, array('video', 'exercise', 'workout_routine'))) {
            $video_id = MFLoadEntities::getEntityFieldValue('node', $node, 'field_jw_player_video_id');
            if ($video_id) {
                return array(
                    '#markup' => '<div class="article-video__mobile"></div>',
                );
            }
        }
        $photo_credit = '';
        $mobile_image = MFLoadEntities::getEntityFieldValue('node', $node, 'field_images');
        if (!$mobile_image) {
            // Show placeholder.
            $mobile_image = file_load(MFBlocks::DEFAULT_IMAGE_FILE_ID);
        }
        else {
            $photo_credit = MFNodeBlocks::preparePhotoCredit($mobile_image);
            if (!$photo_credit) {
                $photo_credit = MFLoadEntities::getEntityFieldValue('node', $node, 'field_photo_credit');
                if ($photo_credit) {
                    $photo_credit = $photo_credit[0];
                }
            }
        }
        $markup = MFBlocks::imageStyleRender($mobile_image, 'node_responce_image');
        if ($photo_credit) {
            $markup .= '<figcaption class="article-media__copy"><svg class="icon svg-icon_camera-dims"><use xmlns:xlink="http://www.w3.org/1999/xlink" xlink:href="#icon_camera"></use></svg>' . $photo_credit . '</figcaption>';
        }

        return array('#markup' => $markup);
    }

  /**
   * Helper to render image style.
   *
   * @param object $file
   *   File object.
   * @param string $image_style
   *   Image style name.
   * @param array $attributes
   *   Image attributes.
   *
   * @return string
   *   Rendered html for image tag.
   */
  public static function imageStyleRender($file, $image_style, $attributes = array()) {
    if (is_array($file) && !empty($file['uri'])) {
      $variables = array(
        'style_name' => $image_style,
        'path' => $file['uri'],
        'alt' => MFLoadEntities::getEntityFieldValue('file', $file['fid'], 'field_file_image_alt_text'),
        'title' => MFLoadEntities::getEntityFieldValue('file', $file['fid'], 'field_file_image_title_text'),
        'width' => !empty($file['width']) ? $file['width'] : NULL,
        'height' => !empty($file['height']) ? $file['height'] : NULL,
        'attributes' => $attributes,
      );

      return theme('image_style', $variables);
    }
  }

  /**
   * Helper to get topics value.
   *
   * @param object|int|string $node
   *   Node object or node id.
   * @param array $attributes
   *   Link attributes array.
   * @param string $type
   *   Flag to determine if we need text or link output.
   *
   * @return string
   *   First node topics field value output (link or text).
   */
  protected static function getTopics($node, $attributes = array(), $type = 'link') {
    // Return sponsored for product nodes.
    if ($node->type == 'product') {
      return t('Sponsored');
    }
    $topics = MFLoadEntities::getEntityFieldValue('node', $node, 'field_topics');
    if (!empty($topics) && is_array($topics)) {
      // Get only first item.
      $topics = reset($topics);
      if (!empty($topics->name) && !empty($topics->tid)) {
        if ($type == 'link') {
          return l($topics->name, 'taxonomy/term/' . $topics->tid, $attributes);
        }
        elseif ($type == 'text') {
          return $topics->name;
        }
      }
    }
  }

  /**
   * Helper to get author value.
   *
   * @param object|int|string $node
   *   Node object or node id.
   * @param array $attributes
   *   Link attributes array.
   * @param string $type
   *   Flag to determine if we need text or link output.
   *
   * @return string
   *   Author field value output.
   */
  protected static function getAuthor($node, $attributes = array(), $type = 'link') {
    $author = MFLoadEntities::getEntityFieldValue('node', $node, 'field_author');
    if (is_object($author)) {
      if (!empty($author->name) && !empty($author->tid)) {
        $author_link_text = t('By') . ' ' . $author->name;
        if ($type == 'link') {
          return l($author_link_text, 'taxonomy/term/' . $author->tid, $attributes);
        }
        elseif ($type == 'text') {
          return $author_link_text;
        }
      }
    }
    else {
      // Fallback on field byline. Can be only text type.
      if ($author = MFLoadEntities::getEntityFieldValue('node', $node, 'field_byline')) {
        return $author;
      }
    }
  }

  /**
   * Helper to get author value.
   *
   * @param object|int|string $node
   *   Node object or node id.
   *
   * @return string
   *   Description field value output.
   */
  protected static function getPromoDescription($node) {
    $description = MFLoadEntities::getEntityFieldValue('node', $node, 'field_promo_description');
    if (empty($description)) {
      $description = MFLoadEntities::getEntityFieldValue('node', $node, 'field_subtitle');
    }

    return $description;
  }

  /**
   * Helper to check if node published today.
   *
   * @param object $node
   *   Node object.
   *
   * @return bool
   *   Flag to detect if node published_at set for today.
   */
  public static function checkTodaysNode($node) {
    $created = MFLoadEntities::getEntityFieldValue('node', $node, 'created');
    if (!empty($created)) {
      $date = date('Y-m-d', $created);
      $today = date('Y-m-d');
      if ($date == $today) {
        return TRUE;
      }
    }

    return FALSE;
  }

  /**
   * Helper to render block.
   *
   * @param string $module
   *   Module name of block to render.
   * @param string $delta
   *   Block delta to render.
   *
   * @return string
   *   Rendered array of block data.
   */
  protected static function renderBlock($module, $delta) {
    if (empty($module)) {
      $module = 'mf_homepage';
    }
    // Process all blocks.
    $block = block_load($module, $delta);
    $render_array = _block_render_blocks(array($block));
    $render_array = _block_get_renderable_array($render_array);
    $block = drupal_render($render_array);


    return $block;
  }

  /**
   * Helper to get term children tids.
   *
   * @param array $tids
   *   Term ids.
   *
   * @return array
   *   Tids array of term children and term id itself.
   */
  protected static function getTermChildrenTids($tids) {
    // We expect array but some h=legacy code can send integer.
    if (!is_array($tids)) {
      $tids = array($tids);
    }

    $term_ids = array();

    foreach ($tids as $tid) {
      $vocabulary = taxonomy_vocabulary_machine_name_load('primary_categories');
      $terms = taxonomy_get_tree($vocabulary->vid, $tid, 1);
      $term_ids[] = (string) $tid;
      foreach ($terms as $t) {
        $term_ids[] = $t->tid;
      }
    }

    return $term_ids;
  }

  /**
   * Helper to get menu renderable array.
   *
   * @param string $name
   *   Menu name string.
   * @param array $ul_classes
   *   Ul list classes array.
   * @param string $li_classes
   *   Li element classes.
   * @param null $svg
   *   Menu icons to apply.
   * @param bool $append_title
   *   Flag to append or not title to svg image.
   *
   * @return array
   *   Menu render array.
   */
  protected static function getMenu($name, $ul_classes, $li_classes, $svg = NULL, $append_title = FALSE) {
    $links = menu_navigation_links($name);
    // Update li elements with needed classes.
    $updated_links = array();
    foreach ($links as $key => $link) {
      if (!empty($link['attributes']['class'])) {
        $network = reset($link['attributes']['class']);
        if (!empty($svg[$network])) {
          if ($append_title) {
            $link['title'] = $svg[$network] . $link['title'];
          }
          else {
            $link['title'] = $svg[$network];
          }
          $link['html'] = TRUE;
        }
      }
      $updated_links[$key . ' ' . $li_classes] = $link;
    }

    return array(
      '#theme' => 'links',
      '#links' => $updated_links,
      '#attributes' => array(
        'class' => $ul_classes,
      ),
    );
  }

  /**
   * Helper to convert string itno css identier.
   *
   * @param string $string
   *   String to convert into css identifier.
   *
   * @return string
   *   Converted identifier.
   */
  protected static function getCssIdentifier($string) {
    return strtolower(drupal_clean_css_identifier($string));
  }

  /**
   * Helper to check quantity of nodes inside nodequeue.
   *
   * @param string $nodequeue
   *   Nodequeue machine name.
   *
   * @return int
   *   Quantity of nodes inside nodequeue.
   */
  protected static function checkIfNodequeueHasNodes($nodequeue) {
    $nodes = MFLoadEntities::getNodequeueNodes($nodequeue);
    return count($nodes);
  }

  /**
   * Helper to get term depth.
   *
   * @param string|int $tid
   *   Term id.
   *
   * @return int
   *   Term depth.
   */
  public static function checkTermDepth($tid) {
    return count(taxonomy_get_parents_all($tid));
  }

  /**
   * Helper to save file object and set status as permanent.
   *
   * @param int|string $fid
   *   File id.
   */
  public static function saveFile($fid, $module) {
    $file = file_load($fid);
    // Change status to permanent.
    $file->status = FILE_STATUS_PERMANENT;
    // Save.
    file_save($file);
    // Record that the module (in this example, user module) is using the file.
    file_usage_add($file, $module, $module . '_image', $fid);
  }

  /**
   * Helper to upload files.
   *
   * @param string $name
   *   File name.
   */
  public static function uploadFile($name, $module) {
    $path = drupal_get_path('module', $module) . '/images/' . $name;
    $file_temp = file_get_contents($path);
    $file = file_save_data($file_temp, 'public://' . $name, FILE_EXISTS_RENAME);
    if (!empty($file->fid)) {
      file_usage_add($file, $module, $module . '_image', $file->fid);
    }

    return $file->fid;
  }

}
