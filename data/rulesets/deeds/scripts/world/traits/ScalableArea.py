import server


# Used for an entity with an area which scales along when the entity scales
class ScalableArea(server.Thing):

    #    def __init__(self, cpp):
    #        init_ticks(self, self.tick_interval)

    def set_operation(self, op):
        # TODO: implement
        return server.OPERATION_IGNORED

# C++ code from Plant
#
#
# void Plant::scaleArea() {
# static float AREA_SCALING_FACTOR=3.0f;
#
# const WFMath::AxisBox<3>& bbox = m_location.bBox();
# if (bbox.isValid()) {
# //If there's an area we need to scale that with the bbox
# auto area_property = modPropertyClassFixed<AreaProperty>();
# if (area_property != nullptr) {
# WFMath::AxisBox<2> footprint = area_property->shape()->footprint();
# //We'll make it so that the footprint of the area is AREA_SCALING_FACTOR times the footprint of the bbox
# auto area_radius = footprint.boundingSphere().radius();
# if (area_radius != 0.0f) {
#
# //We're only interested in the horizontal radius of the plant
# WFMath::AxisBox<2> flat_bbox(WFMath::Point<2>(bbox.lowerBound(0), bbox.lowerBound(2)),
# WFMath::Point<2>(bbox.upperBound(0), bbox.upperBound(2)));
# auto plant_radius = flat_bbox.boundingSphere().radius();
#
# auto desired_radius = plant_radius * AREA_SCALING_FACTOR;
# auto scaling_factor = desired_radius / area_radius;
#
# //No need to alter if the scale is the same.
# //Also don't scale the unless the difference is at least 10% in either direction.
# //The reason for this is that we don't want to alter the area each tick since
# //the client often must perform a sometimes expensive material regeneration
# //calculation every time a terrain area changes. With many plants this runs the
# //risk of bogging down the client then.
# if (!WFMath::Equal(scaling_factor, 1.0f)
# && (scaling_factor > 1.1f || scaling_factor < 0.9f)) {
#     std::unique_ptr<Form<2>> new_area_shape(
#     area_property->shape()->copy());
# new_area_shape->scale(scaling_factor);
# Atlas::Message::MapType shapeElement;
# new_area_shape->toAtlas(shapeElement);
#
# Atlas::Message::Element areaElement;
# area_property->get(areaElement);
# areaElement.asMap()["shape"] = shapeElement;
#
# area_property->set(areaElement);
# area_property->apply(this);
# area_property->addFlags(prop_flag_unsent);
# }
# }
# }
# }
# }
